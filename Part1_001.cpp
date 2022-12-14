//
// Created by xcmworkharder on 2022/08/06
//

#include <iostream>
#include <unordered_map>
//#include <initializer_list>
#include <map>
#include <algorithm>
#include <vector>
#include <functional> // std::function
#include <tuple>

using namespace std;

// 2. decltype
template<typename T, typename U>
decltype((*(T*)0) + (*(U*)0)) Add(T t, U u) {
	return t + u;
}

template<typename T, typename U>
auto Add1(T t, U u)->decltype(t + u) {
	return t + u;
}

// 3. typedef vs using
// 3.1 using typedef
// template<typename Val>
// struct str_map {
// 	typedef map<string, Val> type;
// };

// str_map<int>::type map1; = map<string, int>

// 3.2 using using
// template<typename Val>
// using str_map_t = map<string, Val>;

// 4. Initializer list
struct Foo {
	int x;
	double y;
private:
	static int z;
	//Foo(int, double, int) {}
};

Foo foo {1, 2.0};

class FooMap {
	std::map<int, int> content_;
	using pair_t = std::map<int, int>::value_type;

public:
	FooMap(std::initializer_list<pair_t> list) {
		for (auto it = list.begin(); it != list.end(); ++it) {
			content_.insert(*it);
		}
	}
};

// 5. 定制range类
namespace detail_range {

// 基础迭代器类
template<typename T>
class IIterator {
public:
	using value_type = T;
	using size_type = size_t;

private:
	size_type cursor_;
	const value_type step_;
	value_type value_;

public:
	IIterator(size_type cur_start, value_type begin_val, value_type step_val) 
		: cursor_(cur_start), step_(step_val), value_(begin_val) {
		value_ += (step_ * cursor_);
	}

	// 以下是迭代必备的3个函数
	value_type operator *() const { return value_; }

	bool operator !=(const IIterator& rhs) const {
		return (cursor_ != rhs.cursor_);
	}

	IIterator& operator ++(void) {
		value_ += step_;
		++cursor_;
		return (*this);
	}
};


// range实现类
template<typename T>
class ImplIterator {
public:
	using value_type = T;
	using reference  = const value_type&;
	using const_reference = const value_type&;
	using iterator   = const IIterator<value_type>;
	using const_iterator = const IIterator<value_type>;
	using size_type  = typename IIterator<value_type>::size_type;

private:
	const value_type begin_;
	const value_type end_;
	const value_type step_;
	const size_type max_count_;

	size_type get_adjusted_count(void) const {
		if (step_ > 0 && begin_ >= end_) {
			throw std::logic_error("End value must be greater \
				than begin value.");
		} else if (step_ < 0 && begin_ <= end_) {
			throw std::logic_error("End value mast be less \
				than begin value.");
		}
		
		size_type x = static_cast<size_type>((end_ - begin_) / step_);
		if (begin_ + (step_ * x) != end_) ++x;
		return x;
	}

public:
	ImplIterator(value_type begin_val, value_type end_val, value_type step_val)
		: begin_(begin_val)
		, end_(end_val)
		, step_(step_val)
		, max_count_(get_adjusted_count()) {

	}

	size_type size(void) const {
		return max_count_;
	}

	const_iterator begin(void) const {
		return {0, begin_, step_};
	}

	const_iterator end(void) const {
		return {max_count_, begin_, end_};
	}
};
} // end of detail_range

// range类的外覆函数模板
template<typename T>
detail_range::ImplIterator<T> range(T end) {
	return { {}, end, 1 };
}

template<typename T>
detail_range::ImplIterator<T> range(T begin, T end) {
	return { begin, end, 1 };
}

template<typename T, typename U>
auto range(T begin, T end, U step)
	-> detail_range::ImplIterator<decltype(begin + step)> {
	using r_t = detail_range::ImplIterator<decltype(begin + step)>;
	return r_t(begin, end, step);
}

// 7. 可调用对象
// 7.1 函数
void func71(void) {
	cout << __FUNCTION__ << endl;
}

// 7.2 具有operator()的类（结构体）对象（仿函数）
struct Foo72 {
	void operator()(void) {
		cout << __FUNCTION__ << endl;
	}

	static int foo_func(int a) {
		cout << __FUNCTION__ << a << endl;
		return a;
	}
};

// 7.3 可被转化为函数指针的类对象
struct Bar73 {
	using fr_t = void(*)(void);

	static void func(void) {
		cout << __FUNCTION__ << endl;
	}

	operator fr_t(void) {
		return func;
	}

	int operator()(int a) {
		cout << __FUNCTION__ << a << endl;
		return a;
	}
};

// 7.4 可被调用的类成员指针
struct A74 {
	int a;

	void mem_func(void) {
		cout << __FUNCTION__ << endl;
	}
};

// 8. lambda表达式
class A8 {
public:
	int i_ = 0;

	void func(int x, int y) {
		// auto x1 = [] { return i_; } // 没有捕获外部变量
		auto x2 = [=] { return i_ + x + y; }; // ok
		auto x3 = [&] { return i_ + x + y; }; // ok
		auto x4 = [this] { return i_; }; 	 // ok
		// auto a5 = [this] { return i_ + x + y; } // error 没有捕获x,y
		auto x6 = [this, x, y] { return i_ + x + y; }; // ok
		auto x7 = [this] { return i_++; }; // ok
	}
};

int main() {

	// 9. tuple
	int a = 1;
	string b = "aa";
	int c = 2;
	auto tp = std::tie(a, b, c);
	//auto tp = std::tie(1, 2, "aa"); // error tie(int&, int&, string&)
	int x, y;
	string z;
	tie(std::ignore, std::ignore, y) = tp;
	cout << x << ", " << y << ", " << z << endl;

	tuple<int, string, float> t1(10, "test", 2.3);
	int n = 10;
	auto t2 = tuple_cat(t1, make_pair("Foo", "bar"), t1, tie(n));
	auto test = get<4>(t2);
	cout << test << endl;

	// 8. lambda表达式
	// int a = 0;
	// auto f1 = [=]() mutable { return a++; };
	// std::function<int(int)> f2 = [](int a) { return a; };

	// 7.5 std::function可调用对象包装器
	// 可以容纳除了类成员（函数）指针外所有可调用对象
	// 如下为测试
	// 普通函数
	// std::function<void(void)> fr1 = func71;
	// fr1();

	// // 静态成员函数
	// std::function<int(int)> fr2 = Foo72::foo_func;
	// cout << fr2(123) << endl;

	// // 仿函数
	// Bar73 bar73;
	// fr2 = bar73;
	// cout << fr2(134) << endl;

	// 7. 可调用对象调用
	// void(*func_ptr71)(void) = &func71;
	// func_ptr71();

	// Foo72 foo72;
	// foo72();

	// Bar73 bar73;
	// bar73();

	// void (A74::*mem_func_ptr74)(void) = &A74::mem_func;
	// int A74::*mem_obj_ptr74 = &A74::a;

	// A74 aa74;
	// (aa74.*mem_func_ptr74)();
	// aa74.*mem_obj_ptr74 = 123;

	// 6. 自定义range测试
	// for (auto i : range(2, 6, 1)) {
	// 	cout << " " << i;
	// }
	// cout << endl;

	// 5. range based for
	// vector<int> arr{1, 23, 23};
	// for_each(arr.begin(), arr.end(), [](int n) {
	// 	cout << n << endl;
	// });

	// 4. initializer_list
	//FooMap foo_2 = {{1, 2}, {3, 4}, {5, 6}};
	// 防止类型收窄
	// float b = {1e40}; // error

	// 2. decltype
	//cout << Add1<double, int>(2.2, 2) << endl;

	// 1. auto and unordered_multimap
	// unordered_multimap<int, int> m;
	// m.insert(make_pair(0, 123));
	// m.insert(make_pair(1, 234));
	// m.insert(make_pair(0, 111));

	// auto range = m.equal_range(0);
	// cout << (range.first)->second << (range.second)->second << endl;

	return 0;
}