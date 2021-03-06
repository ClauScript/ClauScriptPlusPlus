

#include "mimalloc-new-delete.h" // comment?

#include <iostream>
#include <ctime>

#include "vm.h"

int main(void)
{
	VM vm;
	int start = clock();
	clau_parser::UserType global;
	clau_parser::LoadData::LoadDataFromFile("C:\\Users\\vztpv\\source\\repos\\ClauScriptPlusPlus\\ClauScriptPlusPlus\\test.txt",
		global, 1, 0); // DO NOT change!

	auto arr = global.GetUserTypeIdx("Event");
	

	for (auto x : arr) {
		//std::cout << x << " ";
		//std::cout << global.GetUserTypeList(x)->ToString() << "\n";
		vm.Register(MakeByteCode(global.GetUserTypeList(x)));
	}
	for (int i = arr.size() - 1; i >= 0; --i) {
		global.RemoveUserTypeList(arr[i]);
	}

	vm.Run("main", &global);

	int last = clock();
	std::cout << last - start << "ms\n";
	
	clau_parser::LoadData::Save(global, "output.eu4");

	return 0;
}

