#pragma once
#include<string>
#include<vector>
#include<cstdarg>
#include<cctype>
#include<algorithm>
namespace PStringUtils
{

	/**
	 * ����ָ���ַ��ָ��ַ���
	 * @param v:���ڴ����ַ�����
	 * @param s:���ָ���ַ���
	 * @param delimiter:�ָ��ַ�
	 * @param maxSegments:����Ӵ��� Ĭ��Ϊ��0x7fffffff Ҳ����ʮ���Ƶ�2,147,483,647
	 * @return �ָ��õ����Ӵ�����
	 * @note v���Ȼᱻ���
	 */
	unsigned int split(std::vector<std::string>& v,
		const std::string& s, char delimiter,
		unsigned int maxSegments = INT_MAX)
	{
		v.clear();
		std::string::size_type left = 0;
		unsigned int i;
		for (i = 1; i < maxSegments; i++) {
			//��left��ʼ�����ҵ��ָ����λ��
			std::string::size_type right = s.find(delimiter, left);
			//��leftһֱ���ַ���ĩβ��û�зָ��
			if (right == std::string::npos) {
				break;
			}
			//�ҵ��˷ָ��ַ����ָ�֮������������Ӧ������λ��
			v.push_back(s.substr(left, right - left));
			//����������ʼλ��
			left = right + 1;
		}
		v.push_back(s.substr(left));
		return i;
	}

	/**
	 * ����ַ���ǰ��հ��ַ�
	 * @param s:Դ�ַ���
	 * @return ǰ����տհ��ַ����ַ���
	 */
	std::string trim(const std::string& s) {
		static const char* whiteSpace = " \t\r\n";
		//�մ�
		if (s.empty())
			return s;
		//�ҵ���һ����Ϊ�������ֿհ��ַ����ַ�������
		std::string::size_type b = s.find_first_not_of(whiteSpace);
		if (b == std::string::npos) // No non-spaces
			return "";
		//�ҵ����һ����Ϊ�հ��ַ����ַ�������
		std::string::size_type e = s.find_last_not_of(whiteSpace);
		//���ؽ�ȡ֮����ַ���
		return std::string(s, b, e - b + 1);
	}

	/**
	 * ����ַ���ǰ��հ��ַ�
	 * @param s:Դ�ַ���
	 * @return ǰ����տհ��ַ����ַ���
	 */
	std::string vform(const char* format, va_list args) {
		// ��һ�ε���vsnprintf��ȡ���軺������С
		va_list args_copy;
		va_copy(args_copy, args); // ����va_list�������һ�ε���Ӱ��ڶ���
		int needed_size = std::vsnprintf(nullptr, 0, format, args_copy);
		va_end(args_copy); // �ͷŵ�һ�ε��õ�args_copy
		// �����㹻��Ļ�����
		if (needed_size <= 0) {
			return std::string();
		}
		std::string result(needed_size + 1, '\0'); // +1��Ϊ�����ɽ�����'\0'
		// �ڶ��ε���vsnprintf����ʵ�ʵĸ�ʽ����ƴ��
		va_copy(args_copy, args); // �ٴθ���va_list
		std::vsnprintf(&result[0], needed_size + 1, format, args_copy);
		va_end(args_copy); // �ͷŵڶ��ε��õ�args_copy
		// ɾ������Ľ�����'\0'
		result.resize(needed_size);
		return result;
	}

	/**
	 * ת��Ϊ��д
	 * @param s:Դ�ַ���
	 * @return ת��Ϊ��д֮����ַ���
	 * @note Դ�ַ����ᱻ�޸�
	 */
	void to_upper(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	}
	/**
	 * ת��Ϊ��д
	 * @param s:Դ�ַ���
	 * @return ת��Ϊ��д֮����ַ���
	 * @note Դ�ַ������ᱻ�޸�
	 */
	std::string to_upper_copy(std::string& str)
	{
		std::string tmp = str;
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
		return tmp;
	}

	/**
 * ת��ΪСд
 * @param s:Դ�ַ���
 * @return ת��ΪСд֮����ַ���
 * @note Դ�ַ����ᱻ�޸�
 */
	void to_lower(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}
	/**
	 * ת��ΪСд
	 * @param s:Դ�ַ���
	 * @return ת��ΪСд֮����ַ���
	 * @note Դ�ַ������ᱻ�޸�
	 */
	std::string to_lower_copy(std::string& str)
	{
		std::string tmp = str;
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
		return tmp;
	}
	/**
	 * ���Դ�Сд���ַ����Ƚ�
	 * @param s1,s2:����Ҫ�Ƚϵ��ַ���
	 * @return ���Դ�Сд֮���Ƿ����
	 */
	bool stringEqualIgnoreCase(const std::string& s1, const std::string& s2) {
		return s1.length() == s2.length() &&
			std::equal(s1.begin(), s1.end(), s2.begin(),
				[](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); });
	}
}
