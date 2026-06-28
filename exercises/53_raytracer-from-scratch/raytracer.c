/* 53_raytracer-from-scratch.c — 从零实现光线追踪，输出 PPM 图像
 *
 * 场景：2 个球体 + 1 个点光源 + 1 个相机
 *   红球：center=(0, 0, -5), r=1
 *   蓝球：center=(1, -0.5, -4), r=0.8
 *   点光源：(0, 10, -3), 白色
 *   相机：原点，朝向 -Z, 像平面 z=-1
 *
 * 任务：
 *   1. 实现向量运算（加减、点积、长度、归一化）
 *   2. 实现光线 - 球体求交（判别式法，返回最小正 t）
 *   3. 实现 Phong 光照（ambient + diffuse + specular）
 *   4. 补全主循环（生成相机光线 + 遍历球体 + 累加光照）
 *
 * 输出：PPM P3 文本格式，64×32 像素，通过 diff 与 expected_output.txt 比对
 *
 * 提示：骨架中的注释会引导你一步步完成。先读完全文再动手！
 */

#include <math.h>
#include <stdio.h>

/* ─── 向量 Vec3 ───
 * 所有向量运算的基础：三维向量 x, y, z。
 * 你需要实现：v3 (构造), v3_add, v3_sub, v3_mul (标量乘),
 *             v3_div (标量除), v3_dot (点积), v3_len (长度), v3_norm (归一化)
 */
typedef struct {
    double x, y, z;
} Vec3;

/* 构造一个向量 */
static inline Vec3 v3(double x, double y, double z) {
    /* 提示：用复合字面量 {x, y, z} 返回 Vec3 */
    Vec3 v = {x, y, z};
    return v;
}

#error TODO: Finish this exercise. Run "clings hint" for help.
/* TODO 1a: 基本向量运算 — 加减、标量乘除 (约 8 行)
 *
 * Vec3 v3_add(Vec3 a, Vec3 b)  — 返回分量和：{a.x+b.x, a.y+b.y, a.z+b.z}
 * Vec3 v3_sub(Vec3 a, Vec3 b)  — 返回分量差：{a.x-b.x, a.y-b.y, a.z-b.z}
 * Vec3 v3_mul(Vec3 a, double s) — 返回标量乘：{a.x*s, a.y*s, a.z*s}
 * Vec3 v3_div(Vec3 a, double s) — 返回标量除：{a.x/s, a.y/s, a.z/s}
 *
 * 提示：用 v3() 构造返回向量。
 */

#error TODO: Finish this exercise. Run "clings hint" for help.
/* TODO 1b: 向量点积、长度、归一化 (约 7 行)
 *
 * double v3_dot(Vec3 a, Vec3 b) — 点积：a.x*b.x + a.y*b.y + a.z*b.z
 * double v3_len(Vec3 v)        — 长度：sqrt(v3_dot(v, v))
 * Vec3 v3_norm(Vec3 v)         — 归一化：v / len  (len>0 时除以长度，否则返回零向量)
 *
 * 注意：v3_len 需要 #include <math.h> (已在顶部), 编译时需要 -lm
 */

/* ─── 光线 Ray ─── */
typedef struct {
    Vec3 orig, dir;
} Ray;

/* 光线上参数 t 处的点：orig + t * dir */
static inline Vec3 ray_at(Ray r, double t) { return v3_add(r.orig, v3_mul(r.dir, t)); }

/* ─── 球体 Sphere ─── */
typedef struct {
    Vec3 center;
    double radius;
    Vec3 color;
} Sphere;

/* ─── 光源 Light ─── */
typedef struct {
    Vec3 pos;
    Vec3 color;
} Light;

/* ─── 场景常量 ─── */
#define WIDTH 64
#define HEIGHT 32

#error TODO: Finish this exercise. Run "clings hint" for help.
/* TODO 2: 光线 - 球体求交 (约 15 行)
 *
 * double hit_sphere(Ray r, Sphere s):
 *   解二次方程 |orig + t*dir - center|^2 = radius^2
 *   oc = orig - center
 *   a = dir·dir
 *   b = 2 * oc·dir
 *   c = oc·oc - radius^2
 *   discriminant = b^2 - 4ac
 *
 *   若 disc < 0: 返回 -1.0 (无交点)
 *   否则计算 t0 = (-b - sqrt(disc)) / (2a)
 *            t1 = (-b + sqrt(disc)) / (2a)
 *   返回两个正根中较小者; 若都 ≤0 则返回 -1.0
 */

/* 将 [0,1] 浮点数转为 0-255 整数 (含 clamp) */
static int to_byte(double x) {
    if (x < 0.0) x = 0.0;
    if (x > 1.0) x = 1.0;
    return (int)(x * 255.0 + 0.5);
}

int main(void) {
    /* 场景：两个球体 */
    Sphere spheres[2] = {
        {v3(0, 0, -5), 1.0, v3(1, 0, 0)},    /* 红球 */
        {v3(1, -0.5, -4), 0.8, v3(0, 0, 1)}, /* 蓝球 */
    };
    int n_spheres = 2;

    /* 点光源 */
    Light light = {v3(0, 10, -3), v3(1, 1, 1)};

    /* 相机参数 */
    Vec3 cam = v3(0, 0, 0);  /* 相机在原点 */
    double viewport_w = 2.0; /* 视口宽度 */
    double viewport_h = 1.0; /* 视口高度 */
    double focal = 1.0;      /* 焦距 (像平面 z = -1) */

    /* 输出 PPM P3 头 */
    printf("P3\n%d %d\n255\n", WIDTH, HEIGHT);

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* TODO 3a: 主循环 — 生成光线并找最近交点 (约 12 行)
     *
     * for (int y = 0; y < HEIGHT; y++):
     *   for (int x = 0; x < WIDTH; x++):
     *
     *     ① 计算光线方向：
     *       u = (x + 0.5) / WIDTH        // [0, 1]
     *       v = (y + 0.5) / HEIGHT
     *       px = (u - 0.5) * viewport_w  // 映射到世界坐标 [-1, 1]
     *       py = (0.5 - v) * viewport_h  // Y 翻转 (PPM 顶行在上)
     *       pz = -focal
     *       Ray ray = { cam, v3_norm(v3(px, py, pz)) };
     *
     *     ② 遍历球体找最近交点：
     *       closest_t = -1.0; hit_idx = -1;
     *       for (int i = 0; i < n_spheres; i++):
     *         t = hit_sphere(ray, spheres[i])
     *         if (t > 0 && (closest_t < 0 || t < closest_t)):
     *           closest_t = t; hit_idx = i;
     */

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* TODO 3b: Phong 光照计算与像素输出 (约 13 行)
     *
     *     ③ 若无交点 (hit_idx < 0):
     *       printf("0 0 0\n");  ← 黑色背景
     *
     *     ④ 若有交点，Phong 光照：
     *       P  = ray_at(ray, closest_t)                    // 交点位置
     *       N  = v3_norm(v3_sub(P, spheres[hit_idx].center)) // 表面法线
     *       obj = spheres[hit_idx].color                    // 物体颜色
     *       L  = v3_norm(v3_sub(light.pos, P))              // 指向光源
     *       V  = v3_norm(v3_sub(cam, P))                    // 指向相机
     *
     *       // Ambient
     *       Vec3 ambient = v3_mul(obj, 0.1);
     *
     *       // Diffuse (Lambertian)
     *       double ndotl = v3_dot(N, L);
     *       Vec3 diffuse = (ndotl > 0) ? v3_mul(obj, 0.7 * ndotl) : v3(0,0,0);
     *
     *       // Specular (Phong)
     *       Vec3 R = v3_sub(v3_mul(N, 2.0 * ndotl), L);    // 反射向量
     *       double rdotv = v3_dot(R, V);
     *       Vec3 specular = (rdotv > 0 && ndotl > 0)
     *                       ? v3_mul(light.color, 0.5 * pow(rdotv, 32.0))
     *                       : v3(0,0,0);
     *
     *       Vec3 color = v3_add(v3_add(ambient, diffuse), specular);
     *
     *     ⑤ 输出：printf("%d %d %d\\n", to_byte(color.x), to_byte(color.y), to_byte(color.z));
     */

    return 0;
}
