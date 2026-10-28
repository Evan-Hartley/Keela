//
// Created by brand on 7/23/2025.
//
// https://en.cppreference.com/w/cpp/error/set_terminate.html
#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
	std::set_terminate([]() {
		std::cout << "Unhandled exception\n" << std::flush;
		std::abort();
	});
	throw 1;
}
