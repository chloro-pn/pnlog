#pragma once
#include <string>

template<class T>
std::string convert_to_string(const T& t) {
  return std::to_string(t);
}

template<>
std::string convert_to_string(const std::string& str);

/*
�������˵��һ�����⣬�������ƶϹ����п��Կ�����ָ���*��ߵ�const����ָ�����ͱ����const�����ǰ�����Ϊ
���ͱ����һ���֡�
��*�ұߵ�const����������ָ�����͵�const����һ�������ƶϡ�
*/
using char_type = const char*;
template<>
std::string convert_to_string(const char_type& ptr);

std::string convert_to_string(const char* ptr);