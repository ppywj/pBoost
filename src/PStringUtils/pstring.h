#pragma once
#include<string>
#include<vector>
#include<cstdarg>
#include<cctype>
#include<algorithm>
namespace PStringUtils
{

	/**
	 * 根据指定字符分割字符串
	 * @param v:用于储存字符串得
	 * @param s:待分割的字符串
	 * @param delimiter:分割字符
	 * @param maxSegments:最大子串数 默认为：0x7fffffff 也就是十进制的2,147,483,647
	 * @return 分割后得到的子串个数
	 * @note v首先会被清空
	 */
	unsigned int split(std::vector<std::string>& v,
		const std::string& s, char delimiter,
		unsigned int maxSegments = INT_MAX)
	{
		v.clear();
		std::string::size_type left = 0;
		unsigned int i;
		for (i = 1; i < maxSegments; i++) {
			//从left开始往后找到分割符的位置
			std::string::size_type right = s.find(delimiter, left);
			//从left一直到字符串末尾都没有分割符
			if (right == std::string::npos) {
				break;
			}
			//找到了分割字符，分割之后存入迭代器对应的容器位置
			v.push_back(s.substr(left, right - left));
			//更新搜索起始位置
			left = right + 1;
		}
		v.push_back(s.substr(left));
		return i;
	}

	/**
	 * 清除字符串前后空白字符
	 * @param s:源字符串
	 * @return 前后清空空白字符的字符串
	 */
	std::string trim(const std::string& s) {
		static const char* whiteSpace = " \t\r\n";
		//空串
		if (s.empty())
			return s;
		//找到第一个不为上面四种空白字符的字符的索引
		std::string::size_type b = s.find_first_not_of(whiteSpace);
		if (b == std::string::npos) // No non-spaces
			return "";
		//找到最后一个不为空白字符的字符的索引
		std::string::size_type e = s.find_last_not_of(whiteSpace);
		//返回截取之后的字符串
		return std::string(s, b, e - b + 1);
	}

	/**
	 * 清除字符串前后空白字符
	 * @param s:源字符串
	 * @return 前后清空空白字符的字符串
	 */
	std::string vform(const char* format, va_list args) {
		// 第一次调用vsnprintf获取所需缓冲区大小
		va_list args_copy;
		va_copy(args_copy, args); // 复制va_list，避免第一次调用影响第二次
		int needed_size = std::vsnprintf(nullptr, 0, format, args_copy);
		va_end(args_copy); // 释放第一次调用的args_copy
		// 分配足够大的缓冲区
		if (needed_size <= 0) {
			return std::string();
		}
		std::string result(needed_size + 1, '\0'); // +1是为了容纳结束符'\0'
		// 第二次调用vsnprintf进行实际的格式化和拼接
		va_copy(args_copy, args); // 再次复制va_list
		std::vsnprintf(&result[0], needed_size + 1, format, args_copy);
		va_end(args_copy); // 释放第二次调用的args_copy
		// 删除多余的结束符'\0'
		result.resize(needed_size);
		return result;
	}

	/**
	 * 转化为大写
	 * @param s:源字符串
	 * @return 转化为大写之后的字符串
	 * @note 源字符串会被修改
	 */
	void to_upper(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	}
	/**
	 * 转化为大写
	 * @param s:源字符串
	 * @return 转化为大写之后的字符串
	 * @note 源字符串不会被修改
	 */
	std::string to_upper_copy(std::string& str)
	{
		std::string tmp = str;
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
		return tmp;
	}

	/**
 * 转化为小写
 * @param s:源字符串
 * @return 转化为小写之后的字符串
 * @note 源字符串会被修改
 */
	void to_lower(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}
	/**
	 * 转化为小写
	 * @param s:源字符串
	 * @return 转化为小写之后的字符串
	 * @note 源字符串不会被修改
	 */
	std::string to_lower_copy(std::string& str)
	{
		std::string tmp = str;
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
		return tmp;
	}
	/**
	 * 忽略大小写的字符串比较
	 * @param s1,s2:两个要比较的字符串
	 * @return 忽略大小写之后是否相等
	 */
	bool stringEqualIgnoreCase(const std::string& s1, const std::string& s2) {
		return s1.length() == s2.length() &&
			std::equal(s1.begin(), s1.end(), s2.begin(),
				[](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); });
	}
}
