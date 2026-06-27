/* <NN>_<NAME>.c — <CHINESE_TITLE>
 *
 * 任务: <WHAT_STUDENT_MUST_IMPLEMENT>
 *       <DETAIL_1>
 *       <DETAIL_2>
 *
 * 知识点: <KNOWLEDGE_POINTS>
 *
 * 验证：
 *   stdin: "<input_line1>\n<input_line2>\n"  → <expected_output>
 *   stdin: "<input_line1>\n<input_line2>\n"  → <expected_output>
 *   stdin: "<input_line1>\n<input_line2>\n"  → <expected_output>
 */
#include <stdio.h>
#include <string.h>

<RETURN_TYPE> <FUNCTION_NAME>(<PARAMETERS>) {
#error TODO: Finish this exercise. Run "clings hint" for help.

}

int main(void)
{
	char buf1[256];
	char buf2[256];

	/* 从 stdin 读取两行 */
	fgets(buf1, sizeof(buf1), stdin);
	fgets(buf2, sizeof(buf2), stdin);

	/* 去掉 fgets 读入的末尾换行符 */
	int i = 0;
	while (buf1[i] && buf1[i] != '\n')
		i++;
	buf1[i] = '\0';
	i = 0;
	while (buf2[i] && buf2[i] != '\n')
		i++;
	buf2[i] = '\0';

	/* TODO: 调用学生实现的函数并打印结果 */

	return 0;
}
