#include <iostream>
#include <chrono>

using namespace std;

string time_string_now() {
	time_t t = time(nullptr);
	tm* local = localtime(&t);
	char buf[64];
	strftime(buf, 64, "%Y-%m-%d_%H:%M:%S", local);
	return buf;
}

int main() {

	cout << time_string_now();

	return 0;
}