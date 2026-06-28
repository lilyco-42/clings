## Lesson 53 — 从零实现基础光线追踪器，输出 PPM 图像【标杆题】

### 课程任务

用纯 C 语言从零实现一个基础光线追踪器（raytracer）：定义场景（2 个球体 + 1 个点光源 + 1 个相机），逐像素发射光线，计算光线 - 球体交点，应用 Phong 光照模型，输出 PPM P3 文本格式的 64×32 彩色图像。通过 `diff` 与 `expected_output.txt` 比对验证。

实现内容包括：向量运算库（7 个函数）、光线 - 球体求交（二次方程判别式法）、Phong 光照（ambient + diffuse + specular）和主循环（相机光线生成 + 球体遍历 + 着色）。

### 前置知识：光线追踪原理

光线追踪是一种**图像合成算法**，模拟光在场景中的物理传播。核心思想是**逆向追踪**：从相机出发，穿过每个像素发射光线，计算光线与场景物体的交点，再根据光照模型确定该像素颜色。

```
                          ┌──────────────────┐
                          │   Image Plane    │
  Camera                  │  (z = -focal)    │
    │                     │                  │
    │                     │  ┌────────────┐  │
    │  Origin (0,0,0)     │  │   Pixel     │  │
    └─────────────────────┤  │  (x, y)     │  │
                          │  │    ↑        │  │
                          │  │  Ray dir    │  │
                          │  │    ↓        │  │
                          │  └────────────┘  │
                          └────────┬─────────┘
                                   │
                                   ▼
                        ┌──────────────────────┐
                        │    Red Sphere        │
                        │  center=(0,0,-5)     │
                        │  radius=1            │
                        │                      │
                        │   ┌─────┐            │
                        │   │ Hit │  ← P(t)    │
                        │   │  Pt │            │
                        │   └─────┘            │
                        └──────────────────────┘
                                   │
                          ┌────────┴────────┐
                          │  Point Light    │
                          │  (0, 10, -3)    │
                          │       │         │
                          │   L = normalize │
                          │   (light - P)   │
                          └────────────────┘
```

**为什么从相机出发？** 从光源出发的大部分光线永远到不了相机，浪费计算。从相机出发只需追踪到达像素的光线，高效得多。

### 场景定义

```
场景元素:
┌──────────────────────────────────────────────────────────────┐
│                                                              │
│   红球: center = (0, 0, -5)   半径 = 1.0   颜色 = (1,0,0)    │
│   蓝球: center = (1, -0.5, -4) 半径 = 0.8  颜色 = (0,0,1)    │
│   点光源: position = (0, 10, -3)  颜色 = (1,1,1) 白色        │
│   相机: origin = (0, 0, 0)  朝向 = -Z  像平面 z = -1          │
│   视口: 宽 2.0 × 高 1.0                                      │
│   分辨率: 64 × 32 像素                                       │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

俯视图（XZ 平面）：

```
              Z = -1        Z = -3        Z = -4      Z = -5
  Camera (0,0)  │             │             │            │
       │        │             │             │            │
       ├────────┤             │    Light    │            │
       │ 像平面  │             │  (0,10,-3)  │            │
       │        │             │    (在 Y=10  │            │
       │        │             │     上方)   │            │
       │        │             │             │            │
       │        ├─────────────┤             │            │
       │        │   光线穿过   │             │            │
       │        │   像素中心   │             │            │
       │        │             │   蓝球      │   红球     │
       │        │             │  (1,-0.5,   │  (0,0,-5)  │
       │        │             │    -4)      │            │
       │        │             │  r=0.8      │  r=1.0     │
       └────────┴─────────────┴─────────────┴────────────┤
                                                        │
                                                        ▼
                                                       -Z
```

### 第一步详解：相机光线生成

每个像素对应一条光线。计算步骤如下：

```
对于像素 (x, y)，其中 x ∈ [0, WIDTH-1], y ∈ [0, HEIGHT-1]:

  Step A: 归一化像素坐标 (加上 0.5 偏移以穿过像素中心)
    u = (x + 0.5) / WIDTH          → u ∈ [0, 1]
    v = (y + 0.5) / HEIGHT         → v ∈ [0, 1]

  Step B: 映射到世界坐标 (视口空间)
    px = (u - 0.5) * viewport_w    → px ∈ [-1, 1]
    py = (0.5 - v) * viewport_h    → py ∈ [-0.5, 0.5] (Y 翻转!)
    pz = -focal                    → pz = -1

  Step C: 归一化为方向向量
    dir = normalize(px, py, pz)

  Ray: origin = (0,0,0), direction = dir
```

**Y 轴翻转**：PPM 格式规定第一行是图像顶部（Y 最大），而屏幕坐标系通常 Y 轴向下。`(0.5 - v)` 实现了翻转。

### 第二步详解：光线 - 球体求交

这是光线追踪的核心几何计算。

```
光线方程:  P(t) = orig + t * dir        (t ≥ 0)
球体方程:  |P - center|^2 = radius^2

代入光线方程:
  |orig + t*dir - center|^2 = radius^2

令 oc = orig - center:
  |oc + t*dir|^2 = radius^2
  (oc + t*dir)·(oc + t*dir) = radius^2
  oc·oc + 2t*oc·dir + t^2*dir·dir - radius^2 = 0

得到标准二次方程: a*t^2 + b*t + c = 0
  其中:
    a = dir·dir        (= 1.0 若 dir 已归一化)
    b = 2 * oc·dir
    c = oc·oc - radius^2

判别式: discriminant = b^2 - 4*a*c
```

```
判别式的三种情况:

  disc < 0:  无交点 → 光线与球体不相交
              ┌───┐
              │ S │  ← 光线从旁经过
              └───┘
              ←──── ray

  disc = 0:  相切 → 光线擦过球体表面
              ┌───┐
              │ S │  ← 一个交点
              └───┤
              ←──── ray

  disc > 0:  相交 → 光线穿过球体 (两个交点)
              ┌───┐
              │ S │  ← t0 (近) 和 t1 (远)
          ┌───┤   ├───┐
          │   └───┘   │
          │  t0   t1  │
          └────────────┘
              ←──── ray
```

```
求根公式:
  sqrt_disc = sqrt(disc)
  t0 = (-b - sqrt_disc) / (2*a)    ← 较小的根
  t1 = (-b + sqrt_disc) / (2*a)    ← 较大的根

交点选择:
  if t0 > 0: return t0    ← 最近的可见交点
  if t1 > 0: return t1    ← 相机在球体内部时
  otherwise: return -1.0  ← 两个交点都在相机后面
```

**为什么需要两个正根？** 如果相机在球体内部，t0 为负（在相机后面），t1 为正（前方出口）。取最小的正 t 得到最近的可见表面。

### 第三步详解：Phong 光照模型

Phong 模型将表面反射分解为三个分量：

```
I = I_ambient + I_diffuse + I_specular

        光源
         ★
        /|\
       / | \
      /  |  \  ← 入射光方向 L
     /   |   \
    /    |    \
   /     N     \      ← 表面法线 N
  /      |      \
 /       |       \     ← 反射方向 R
▼────────●────────▼
  表面    P     表面
         │
         ▼
       相机 V
```

#### 分量 1: Ambient (环境光)

```
I_ambient = k_a * object_color

k_a = 0.1  (环境光系数)

作用: 模拟间接光照，防止阴影区域全黑。
      即使是场景中没有直接光照的区域，也会有一点亮度。
```

#### 分量 2: Diffuse (漫反射，Lambertian)

```
I_diffuse = k_d * object_color * max(0, N·L)

k_d = 0.7  (漫反射系数)
N = 表面法线 (单位向量)
L = 指向光源的单位向量

作用: 表面亮度与光线入射角的余弦成正比 (Lambert 余弦定律)。
      正对光源的面最亮，侧面渐暗，背面全黑。
```

```
N·L 的几何意义:

  N·L = cos(θ)  (N 和 L 都是单位向量)

      N  ↑
         │   ↗ L
         │  /
         │θ/
         │/
    ─────●───── 表面
         │
    θ=0°  → N·L=1  → 最亮
    θ=90° → N·L=0  → 最暗
    θ>90° → N·L<0 → clamp 到 0 (背面)
```

#### 分量 3: Specular (高光，Phong)

```
R = 2 * (N·L) * N - L       ← 反射向量 (入射光关于法线的镜像)
V = normalize(camera - P)   ← 指向相机的单位向量

I_specular = k_s * light_color * pow(max(0, R·V), shininess)

k_s = 0.5       (高光系数)
shininess = 32  (高光指数)

作用: 模拟镜面反射的高光亮点。
      R·V 越大 → 反射光越接近视线方向 → 越亮。
      pow(..., 32) 使亮点集中在反射方向附近的小区域。
```

```
反射向量的推导:

  入射光 L 关于法线 N 的反射:
    将 L 分解为平行于 N 和垂直于 N 的分量
    L_parallel = (L·N) * N
    L_perp     = L - L_parallel

    反射光 R 的平行分量与入射光相同，垂直分量取反:
    R_parallel = L_parallel  = (L·N) * N
    R_perp     = -L_perp     = -(L - (L·N)*N)

    R = R_parallel + R_perp
      = (L·N)*N - (L - (L·N)*N)
      = 2*(L·N)*N - L
```

```
高光指数的影响:

  shininess=1:     pow(x, 1)   → 宽泛的亮区
  shininess=16:    pow(x, 16)  → 中等亮点
  shininess=32:    pow(x, 32)  → 集中的高光
  shininess=128:   pow(x, 128) → 非常锐利的亮点

  指数越大，表面看起来越"光滑"。
```

### 法线计算

球体表面某点 P 的法线方向是从球心指向 P：

```
N = normalize(P - center)

        center
          ●
          │
          │  N (法线)
          │
          ▼
          P (表面交点)
```

**为什么要归一化？** 光照计算中的点积 N·L 需要单位向量才能得到正确的余弦值。

### 完整渲染流程

```
for y = 0 to HEIGHT-1:
    for x = 0 to WIDTH-1:
        ① 生成光线: pixel → world → ray direction
        ② 找最近交点: 遍历所有球体, 取最小正 t
        ③ if 无交点:
              输出黑色 (0, 0, 0)
           else:
              计算 P, N, L, V, R
              ambient  = obj_color * 0.1
              diffuse  = obj_color * 0.7 * max(0, N·L)
              specular = white * 0.5 * pow(max(0, R·V), 32)
              color = ambient + diffuse + specular
              输出 to_byte(color)
```

### 数据结构总览

```
Vec3 { double x, y, z }
  ├── v3(x,y,z)           构造
  ├── v3_add(a,b)         加法
  ├── v3_sub(a,b)         减法
  ├── v3_mul(v,s)         标量乘
  ├── v3_div(v,s)         标量除
  ├── v3_dot(a,b)         点积
  ├── v3_len(v)           长度
  └── v3_norm(v)          归一化

Ray  { Vec3 orig, Vec3 dir }
  └── ray_at(r, t)        orig + t*dir

Sphere { Vec3 center; double radius; Vec3 color; }

Light  { Vec3 pos; Vec3 color; }
```

### PPM P3 格式

```
P3              ← 魔数: ASCII 彩色图像
64 32           ← 宽度 高度
255             ← 最大颜色值
203 0 0         ← 像素(0,0): R=203 G=0 B=0
0 0 0           ← 像素(1,0): 黑色背景
...
                ← 共 WIDTH×HEIGHT 行像素数据
```

PPM (Portable Pixmap) 是最简单的图像格式之一，纯文本，可以直接用 `cat` 查看、用 `diff` 比对。P3 表示 ASCII 格式（还有 P6 二进制格式）。

### 常见错误与陷阱

| 错误                  | 为什么错                       | 正确做法                      |
| --------------------- | ------------------------------ | ----------------------------- |
| 忘加 `-lm`            | `sqrt()` 和 `pow()` 在 libm 中 | Makefile 中 LDLIBS = -lm      |
| Y 轴未翻转            | PPM 顶行对应世界坐标 Y 最大    | `py = (0.5 - v) * viewport_h` |
| 像素坐标不加 0.5 偏移 | 光线穿过像素角而非中心         | `(x + 0.5) / WIDTH`           |
| hit_sphere 返回负 t   | 交点在相机后方，不可见         | 只返回 >0 的根                |
| 两个正根取大的        | 远处的交点被近处遮挡           | 取最小正 t                    |
| N·L 或 R·V 未 clamp   | 负值使颜色变暗甚至变负         | 用 `if (>0)` 或 `max(0, ...)` |
| 法线方向反了          | P-center 而非 center-P         | N = normalize(P - center)     |
| 反射公式写反          | 符号错误导致高光位置偏移       | R = 2*(N·L)*N - L             |
| to_byte 不 clamp      | >1.0 的值使 printf 输出 >255   | clamp 到 [0,1] 再乘 255       |
| diff 比对失败         | 浮点精度导致微小差异           | 确保计算顺序和公式完全一致    |

### 重要知识点

- **光线追踪基本循环**: 相机→像素→光线→求交→着色，这是所有光线追踪器的骨架
- **二次方程判别式**: `b^2 - 4ac` 是判断光线与球体位置关系的核心
- **Phong 光照三分量**: Ambient（基底亮度）+ Diffuse（漫反射）+ Specular（高光）= 完整表面外观
- **Lambert 余弦定律**: 表面亮度正比于光线与法线夹角的余弦，即 N·L
- **反射向量**: R = 2*(N·L)*N - L，镜面高光的基础
- **PPM P3 格式**: 最简单的图像格式，纯文本，易于生成和比对
- **坐标系统**: 世界坐标 (3D) → 视口坐标 (2D 投影) → 像素坐标 (离散)，三步映射

### 课堂讨论

1. **为什么用光线追踪而不是光栅化？** 光线追踪天然支持反射、折射、阴影等全局光照效果，代码逻辑直观。光栅化是实时渲染的主流，但全局效果需要额外技巧。

2. **Phong 和 Blinn-Phong 有什么区别？** Blinn-Phong 用半角向量 `H = normalize(L+V)` 代替反射向量 `R`，计算 `N·H` 而非 `R·V`，在计算上更高效，高光形状略有不同。本题使用经典 Phong。

3. **如果再加一个球体会怎样？** 只需在 `spheres[]` 数组中添加一个元素，`n_spheres` 改为 3。代码无需其他修改——这是数据驱动设计的优势。

4. **怎么添加阴影？** 从交点向光源发一条"阴影光线"，若在到达光源之前碰到其他物体，则该点处于阴影中。阴影中的点只保留 ambient 分量。

5. **为什么分辨率这么低？** 64×32 = 2048 条光线，在纯 CPU 上几乎瞬间完成。教学目的：小图像便于肉眼检查 PPM 文本输出，也方便 diff 比对。提升分辨率只需改 WIDTH/HEIGHT。

6. **这个光线追踪器能渲染反射材质吗？** 目前不能，但扩展很简单：在交点处递归发射反射光线，将其颜色乘以反射系数加到当前像素。这就是递归光线追踪（Whitted 1980）。

7. **PPM 为什么不直接用 PNG/BMP？** PPM 是纯文本格式，`printf` 直接输出，无需任何图像库。教学场景下零依赖是最佳选择。

### 向量运算速查表

| 操作   | 公式                          | 代码          |
| ------ | ----------------------------- | ------------- |
| 加法   | a + b = (ax+bx, ay+by, az+bz) | `v3_add(a,b)` |
| 减法   | a - b = (ax-bx, ay-by, az-bz) | `v3_sub(a,b)` |
| 标量乘 | a * s = (ax*s, ay*s, az*s)    | `v3_mul(a,s)` |
| 标量除 | a / s = (ax/s, ay/s, az/s)    | `v3_div(a,s)` |
| 点积   | a·b = ax*bx+ay*by+az*bz       | `v3_dot(a,b)` |
| 长度   | \|v\| = sqrt(v·v)             | `v3_len(v)`   |
| 归一化 | v / \|v\|                     | `v3_norm(v)`  |

### 后续衔接

- Lesson 51: 表驱动 LL(1) 解析器（编译器前端）
- Lesson 52: B+ 树索引（数据结构）
- Lesson 54: 更多光线追踪扩展（反射、折射、阴影）
- 高阶延伸：路径追踪 (Path Tracing)、蒙特卡洛积分、BVH 加速结构

### 参考资料

- Turner Whitted. "An Improved Illumination Model for Shaded Display" (1980) — 递归光线追踪的奠基论文
- Bui Tuong Phong. "Illumination for Computer Generated Pictures" (1975) — Phong 光照模型原论文
- Peter Shirley. "Ray Tracing in One Weekend" — 现代入门教程
- 《计算机图形学》(Foley et al.) 第 16 章 — 光照模型与着色
- Wikipedia: Phong reflection model — https://en.wikipedia.org/wiki/Phong_reflection_model
- Wikipedia: Ray tracing (graphics) — https://en.wikipedia.org/wiki/Ray_tracing_(graphics)
- Netpbm (PPM format) — https://en.wikipedia.org/wiki/Netpbm
