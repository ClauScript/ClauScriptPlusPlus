
#include "vm.h"


bool VM::EqualFunc(clau_parser::UserType* global, const clau_parser::ItemType<std::string>& x,
	clau_parser::ItemType<std::string> y, long long x_idx, const std::string& dir, VM* vm) {

	auto x_name = x.GetName();
	auto x_value = x.Get();

	bool use_not = false;
	if (y.Get()._Starts_with("!"sv)) {
		use_not = true;
		y.Set(0, y.Get().substr(1));
	}

	{
		std::string name = y.GetName();

		if (name._Starts_with("&"sv) && name.size() >= 2) {
			long long idx = std::stoll(name.substr(1));
			if (idx < 0 || idx >= global->GetItemListSize()) {
				return false;
			}

			if (y.Get() == "%any"sv) {
				return !use_not;
			}

			x_idx = idx;
			x_name = global->GetItemList(idx).GetName();
			x_value = global->GetItemList(idx).Get();
		}
	}

	if (y.Get() == "%any"sv) {
		return !use_not;
	}

	if (y.Get()._Starts_with("%event_"sv)) {
		std::string event_id = y.Get().substr(7);

		std::unordered_map<std::string, Token> param;

		Token token;
		token.SetString(x_name.empty() ? "EMPTY_NAME" : x_name);
		param["name"] = token;
		token.SetString(x_value);
		param["value"] = token;
		token.SetBool(false);
		param["is_user_type"] = token;

		std::string result = vm->Run(event_id, global, param)[0].ToString();

		bool success = false;
		if (result == "TRUE"sv) {
			success = true;
		}

		if (use_not) {
			return !success;
		}
		return success;
	}

	if (use_not) {
		return x_value != y.Get();
	}
	return x_value == y.Get();
}


bool VM::_InsertFunc(clau_parser::UserType* global, clau_parser::UserType* insert_ut, VM* vm) {
	std::queue<UtInfo> que;

	std::string dir = "/.";

	que.push(UtInfo(global, insert_ut, dir));

	while (!que.empty()) {
		UtInfo x = que.front();
		que.pop();

		// find non-@
		long long ut_count = 0;
		long long it_count = 0;
		long long count = 0;

		for (long long i = 0; i < x.ut->GetIListSize(); ++i) {
			if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
				&& !x.ut->GetItemList(it_count).Get()._Starts_with("@"sv)) {
				// chk exist all case of value?
				auto item = x.global->GetItemIdx("");
				// no exist -> return false;
				if (item.empty()) {
					// LOG....
					return false;
				}

				bool pass = false;
				for (long long j = 0; j < item.size(); ++j) {
					if (EqualFunc(x.global, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
						x.dir, vm)) {
						pass = true;
						break;
					}
				}
				if (!pass) {
					return false;
				}
			}
			else if (x.ut->IsItemList(i) && !x.ut->GetItemList(it_count).GetName().empty() && !x.ut->GetItemList(it_count).GetName()._Starts_with("@"sv)) {
				// chk exist all case of value?
				auto item = x.global->GetItemIdx(x.ut->GetItemList(it_count).GetName());
				// no exist -> return false;
				if (item.empty()) {
					// LOG....
					return false;
				}

				bool pass = false;

				for (long long j = 0; j < item.size(); ++j) {
					if (EqualFunc(x.global, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
						x.dir, vm)) {
						pass = true;
						break;
					}
				}
				if (!pass) {
					return false;
				}

			}
			else if (x.ut->IsUserTypeList(i) && !x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("@"sv)) {
				// chk all case exist of @.
				// no exist -> return false;
				if (x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("$"sv)) {
					ut_count++;
					count++;
					continue;
				}

				auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

				if (usertype.empty()) {
					return false;
				}

				ut_count++;
				count++;

				for (long long j = 0; j < usertype.size(); ++j) {
					que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count - 1),
						x.dir));
				}

				continue;
			}

			if (x.ut->IsItemList(i)) {
				it_count++;
			}
			else {
				ut_count++;
			}
			count++;
		}
	}

	return true;
}

bool VM::_RemoveFunc(clau_parser::UserType* global, clau_parser::UserType* insert_ut, VM* vm) {
	std::queue<UtInfo> que;
	std::string dir = "/.";
	que.push(UtInfo(global, insert_ut, dir));

	while (!que.empty()) {
		UtInfo x = que.front();
		que.pop();

		// find non-@
		long long ut_count = 0;
		long long it_count = 0;
		long long count = 0;

		for (long long i = 0; i < x.ut->GetIListSize(); ++i) {
			if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
				&& !x.ut->GetItemList(it_count).Get()._Starts_with("@"sv)) {
				// chk exist all case of value?
				auto item = x.global->GetItemIdx("");
				// no exist -> return false;
				if (item.empty()) {
					// LOG....
					return false;
				}

				bool pass = false;
				for (long long j = 0; j < item.size(); ++j) {
					if (EqualFunc(x.global, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
						x.dir, vm)) {
						pass = true;
						break;
					}
				}
				if (!pass) {
					return false;
				}
			}
			else if (x.ut->IsItemList(i) && !x.ut->GetItemList(it_count).GetName().empty() && !x.ut->GetItemList(it_count).GetName()._Starts_with("@"sv)) {
				// chk exist all case of value?
				auto item = x.global->GetItemIdx(x.ut->GetItemList(it_count).GetName());
				// no exist -> return false;
				if (item.empty()) {
					if (x.ut->GetItemList(it_count).Get() == "!%any") {
						return true;
					}
					// LOG....
					return false;
				}

				bool pass = false;

				for (long long j = 0; j < item.size(); ++j) {
					if (EqualFunc(x.global, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
						x.dir, vm)) {
						pass = true;
						break;
					}
				}
				if (!pass) {
					return false;
				}

			}
			else if (x.ut->IsUserTypeList(i) && !x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("@"sv)) {
				// chk all case exist of @.
				// no exist -> return false;
				if (x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("$"sv)) {
					ut_count++;
					count++;
					continue;
				}

				auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

				if (usertype.empty()) {
					return false;
				}

				ut_count++;
				count++;

				for (long long j = 0; j < usertype.size(); ++j) {
					que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count - 1),
						x.dir + "/$ut" + std::to_string(usertype[j])));
				}

				continue;
			}
			else if (x.ut->IsUserTypeList(i) && x.ut->GetUserTypeList(ut_count)->GetName() == "@$"sv) {
				//
			}

			if (x.ut->IsItemList(i)) {
				it_count++;
			}
			else {
				ut_count++;
			}
			count++;
		}
	}

	return true;
}


bool VM::_UpdateFunc(clau_parser::UserType* global, clau_parser::UserType* insert_ut, VM* vm) {
	std::queue<UtInfo> que;
	std::string dir = "/.";
	que.push(UtInfo(global, insert_ut, dir));

	while (!que.empty()) {
		UtInfo x = que.front();
		que.pop();


		// find non-@
		long long ut_count = 0;
		long long it_count = 0;
		long long count = 0;

		for (long long i = 0; i < x.ut->GetIListSize(); ++i) {
			if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
				&& !x.ut->GetItemList(it_count).Get()._Starts_with("@"sv)) {
				// chk exist all case of value?
				auto item = x.global->GetItemIdx("");
				// no exist -> return false;
				if (item.empty()) {
					// LOG....
					return false;
				}

				bool pass = false;
				for (long long j = 0; j < item.size(); ++j) {
					if (EqualFunc(x.global, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
						x.dir, vm)) {
						pass = true;
						break;
					}
				}
				if (!pass) {
					return false;
				}
			}
			else if (x.ut->IsItemList(i) && !x.ut->GetItemList(it_count).GetName().empty() && !x.ut->GetItemList(it_count).GetName()._Starts_with("@"sv)) {
				// chk exist all case of value?
				auto item = x.global->GetItemIdx(x.ut->GetItemList(it_count).GetName());
				// no exist -> return false;
				if (item.empty()) {
					// LOG....
					return false;
				}

				bool pass = false;

				for (long long j = 0; j < item.size(); ++j) {
					if (EqualFunc(x.global, x.global->GetItemList(item[j]), x.ut->GetItemList(it_count), item[j],
						dir, vm)) {
						pass = true;
						break;
					}
				}
				if (!pass) {
					return false;
				}

			}
			else if (x.ut->IsUserTypeList(i) && !x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("@"sv)) {
				// chk all case exist of @.
				// no exist -> return false;
				if (x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("$"sv)) {
					ut_count++;
					count++;
					continue;
				}

				auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

				if (usertype.empty()) {
					return false;
				}

				ut_count++;
				count++;

				for (long long j = 0; j < usertype.size(); ++j) {
					que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count - 1),
						dir + "$ut" + std::to_string(usertype[j])));
				}

				continue;
			}

			if (x.ut->IsItemList(i)) {
				it_count++;
			}
			else {
				ut_count++;
			}
			count++;
		}
	}

	return true;
}

// starts with '@' -> insert target
// else -> condition target.
bool VM::InsertFunc(clau_parser::UserType* global, clau_parser::UserType* insert_ut, VM* vm) {
	if (!_InsertFunc(global, insert_ut, vm)) {
		return false;
	}
	std::string dir = "/.";
	std::queue<UtInfo> que;
	// insert
	que.push(UtInfo(global, insert_ut, dir));

	while (!que.empty()) {
		UtInfo x = que.front();
		que.pop();

		// find non-@
		long long ut_count = 0;
		long long it_count = 0;
		long long count = 0;

		//chk only @  ! - todo
		for (long long i = 0; i < x.ut->GetIListSize(); ++i) {
			if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
				&& x.ut->GetItemList(it_count).Get()._Starts_with("@"sv)) {

				if (x.ut->GetItemList(it_count).Get()._Starts_with("@%event_"sv)) {
					std::string event_id = x.ut->GetItemList(it_count).Get().substr(8);

					std::unordered_map<std::string, Token> param;
					Token token;
					token.SetString("EMPTY_STRING");
					param["name"] = token;
					token.SetBool(true);
					param["is_user_type"] = token;

					std::string result = vm->Run(event_id, global, param)[0].ToString();

					x.global->AddItem("", result);
				}
				else {
					x.global->AddItem("", x.ut->GetItemList(it_count).Get().substr(1));
				}

				it_count++;
			}
			else if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName()._Starts_with("@"sv)) {
				if (x.ut->GetItemList(it_count).Get()._Starts_with("%event_"sv)) {
					std::string event_id = x.ut->GetItemList(it_count).Get().substr(7);

					std::unordered_map<std::string, Token> param;
					Token token;
					token.SetString(x.ut->GetItemList(it_count).GetName());
					param["name"] = token;
					token.SetBool(true);
					param["is_user_type"] = token;

					std::string result = vm->Run(event_id, global, param)[0].ToString();

					x.global->AddItem(
						x.ut->GetItemList(it_count).GetName().substr(1),
						result);
				}
				else {
					x.global->AddItem(
						x.ut->GetItemList(it_count).GetName().substr(1),
						x.ut->GetItemList(it_count).Get());
				}
				it_count++;
			}
			else if (x.ut->IsUserTypeList(i) && x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("@"sv)) {
				x.global->LinkUserType(x.ut->GetUserTypeList(ut_count));
				x.ut->GetUserTypeList(ut_count)->SetName(x.ut->GetUserTypeList(ut_count)->GetName().substr(1));
				x.ut->GetUserTypeList(ut_count) = nullptr;

				x.ut->RemoveUserTypeList(ut_count);
				count--;
				i--;
			}
			else if (x.ut->IsUserTypeList(i) && !x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("@"sv)) {
				if (x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("$"sv)) {
					auto temp = x.ut->GetUserTypeList(ut_count)->GetName();
					auto name = temp.substr(1);

					if (name.empty()) {

						for (long long j = 0; j < x.global->GetUserTypeListSize(); ++j) {
							if (_InsertFunc(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count), vm)) {
								que.push(UtInfo(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count)
									, x.dir + "/$ut" + std::to_string(j)
								));
							}
						}
					}
					else {

						auto usertype = x.global->GetUserTypeItemIdx(name);

						for (long long j = 0; j < usertype.size(); ++j) {
							if (_InsertFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), vm)) {
								que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
									x.dir + "/$ut" + std::to_string(usertype[j])));
							}
						}
					}
				}
				else {
					auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

					for (long long j = 0; j < usertype.size(); ++j) {
						//if (_InsertFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), eventUT)) {
						que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
							x.dir + "/$ut" + std::to_string(usertype[j])));
						//}
					}
				}
				ut_count++;
			}
			else if (x.ut->IsUserTypeList(i)) {
				ut_count++;
			}
			else {
				it_count++;
			}

			count++;
		}
	}

	return true;
}

bool VM::RemoveFunc(clau_parser::UserType* global, clau_parser::UserType* insert_ut, VM* vm) {
	if (!_RemoveFunc(global, insert_ut, vm)) {
		return false;
	}
	std::string dir = "/.";
	std::queue<UtInfo> que;
	// insert
	que.push(UtInfo(global, insert_ut, dir));

	while (!que.empty()) {
		UtInfo x = que.front();
		que.pop();

		// find non-@
		long long ut_count = x.ut->GetUserTypeListSize() - 1;
		long long it_count = x.ut->GetItemListSize() - 1;
		long long count = x.ut->GetIListSize() - 1;

		//chk only @  ! - todo
		for (long long i = x.ut->GetIListSize() - 1; i >= 0; --i) {
			if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
				&& x.ut->GetItemList(it_count).Get()._Starts_with("@"sv)) {

				if (x.ut->GetItemList(it_count).Get()._Starts_with("@%event_"sv)) {
					std::string event_id = x.ut->GetItemList(it_count).Get().substr(8);

					auto temp = x.global->GetItemIdx("");

					for (long long j = 0; j < temp.size(); ++j) {
						std::unordered_map<std::string, Token> param;

						Token token;
						token.SetString("EMPTY_STRING");
						param["name"] = token;
						token.SetString(x.global->GetItemList(temp[j]).Get());
						param["value"] = token;
						token.SetBool(false);
						param["is_user_type"] = token;

						std::string result = vm->Run(event_id, global, param)[0].ToString();


						if (result == "TRUE"sv) { //x.ut->GetItemList(it_count).Get().substr(1)) {
							x.global->RemoveItemList(temp[j]);
						}
					}
				}
				else {
					x.global->RemoveItemList("", x.ut->GetItemList(it_count).Get().substr(1));
				}
				it_count--;
				//x.global->AddItemType(clau_parser::ItemType<std::string>("", x.ut->GetItemList(it_count).Get().substr(1)));
			}
			else if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName()._Starts_with("@"sv)) {
				//x.global->AddItemType(clau_parser::ItemType<std::string>(
				//	x.ut->GetItemList(it_count).GetName().substr(1),
				//	x.ut->GetItemList(it_count).Get()));
				if (x.ut->GetItemList(it_count).Get()._Starts_with("%event_"sv)) {
					std::string event_id = x.ut->GetItemList(it_count).Get().substr(7);

					auto name = x.ut->GetItemList(it_count).GetName().substr(1);
					auto temp = x.global->GetItemIdx(name);

					if (name._Starts_with("&"sv) && name.size() >= 2) {
						long long idx = std::stoll(name.substr(1));

						if (idx < 0 || idx >= x.global->GetItemListSize()) { // .size()) {
							return false;
						}
						auto valName = x.ut->GetItemList(it_count).Get();

						std::unordered_map<std::string, Token> param;
						Token token;
						token.SetString(name);
						param["name"] = token;
						token.SetString(x.global->GetItemList(idx).Get());
						param["value"] = token;
						token.SetBool(false);
						param["is_user_type"] = token;

						std::string result = vm->Run(event_id, global, param)[0].ToString();


						if (result == "TRUE"sv) {
							x.global->RemoveItemList(idx);
						}
					}
					else {
						for (long long j = 0; j < temp.size(); ++j) {
							std::unordered_map<std::string, Token> param;
							Token token;
							token.SetString(name);
							param["name"] = token;
							token.SetString(x.global->GetItemList(temp[j]).Get());
							param["value"] = token;
							token.SetBool(false);
							param["is_user_type"] = token;

							std::string result = vm->Run(event_id, global, param)[0].ToString();


							if (result == "TRUE"sv) {
								x.global->RemoveItemList(temp[j]);
							}
						}
					}
				}
				else {
					x.global->RemoveItemList(x.ut->GetItemList(it_count).GetName().substr(1), x.ut->GetItemList(it_count).Get());
				}

				it_count--;
			}
			else if (x.ut->IsUserTypeList(i) && x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("@"sv)) {
				if (x.ut->GetUserTypeList(ut_count)->GetName() == "@$"sv) {
					for (long long j = x.global->GetUserTypeListSize() - 1; j >= 0; --j) {
						if (_RemoveFunc(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count), vm)) {
							delete[] x.global->GetUserTypeList(j);
							x.global->GetUserTypeList(j) = nullptr;
							x.global->RemoveUserTypeList(j);
						}
					}
				}
				else {
					auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName().substr(1));

					for (long long j = usertype.size() - 1; j >= 0; --j) {
						if (_RemoveFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), vm)) {
							x.global->RemoveUserTypeList(usertype[j]);
						}
					}
				}
				ut_count--;
			}
			else if (x.ut->IsUserTypeList(i) && false == x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("@"sv)) {
				if (x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("$"sv)) {
					auto temp = x.ut->GetUserTypeList(ut_count)->GetName();
					auto name = temp.substr(1);

					if (name.empty()) {

						for (long long j = 0; j < x.global->GetUserTypeListSize(); ++j) {
							if (_InsertFunc(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count), vm)) {
								que.push(UtInfo(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count)
									, x.dir + "/$ut" + std::to_string(j)
								));
							}
						}
					}
					else {
						auto usertype = x.global->GetUserTypeItemIdx(name);

						for (long long j = 0; j < usertype.size(); ++j) {
							if (_InsertFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), vm)) {
								que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
									x.dir + "/$ut" + std::to_string(usertype[j])));
							}
						}
					}
				}
				else {
					auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

					for (long long j = 0; j < usertype.size(); ++j) {
						//if (_RemoveFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), eventUT)) {
						que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
							x.dir + "/$ut" + std::to_string(usertype[j])));
						//}
					}
				}

				ut_count--;
			}
			else if (x.ut->IsUserTypeList(i)) {
				ut_count--;
			}
			else if (x.ut->IsItemList(i)) {
				it_count--;
			}

			count--;
		}
	}

	return true;
}

bool VM::UpdateFunc(clau_parser::UserType* global, clau_parser::UserType* insert_ut, VM* vm) {
	if (!_UpdateFunc(global, insert_ut, vm)) {
		return false;
	}
	std::string dir = "/.";
	std::queue<UtInfo> que;
	// insert
	que.push(UtInfo(global, insert_ut, dir));

	while (!que.empty()) {
		UtInfo x = que.front();
		que.pop();

		// find non-@
		long long ut_count = 0;
		long long it_count = 0;
		long long count = 0;

		//chk only @  ! - todo
		for (long long i = 0; i < x.ut->GetIListSize(); ++i) {
			if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName().empty()
				&& x.ut->GetItemList(it_count).Get()._Starts_with("@"sv)) {
				// think @&0 = 3 # 0 <- index, 3 <- value.
				//x.global->GetItemList(0).Set(0, x.ut->GetItemList(it_count).Get());
			}
			else if (x.ut->IsItemList(i) && x.ut->GetItemList(it_count).GetName()._Starts_with("@"sv)) {
				if (x.ut->GetItemList(it_count).Get()._Starts_with("%event_"sv)) {
					std::string event_id = x.ut->GetItemList(it_count).Get().substr(7);

					std::string name = x.ut->GetItemList(it_count).GetName().substr(1);
					auto position = x.global->GetItemIdx(name);

					{
						std::string name = x.ut->GetItemList(it_count).GetName().substr(1);
						if (name._Starts_with("&"sv) && name.size() >= 2) {
							long long idx = std::stoll(name.substr(1));
							if (idx < 0 || idx >= x.global->GetItemListSize()) {
								return false; // error
							}
							else {
								position.clear();
								position.push_back(idx);
							}
						}

						for (long long j = 0; j < position.size(); ++j) {
							std::unordered_map<std::string, Token> param;
							Token token;
							token.SetString(x.ut->GetItemList(it_count).GetName().substr(1));
							param["name"] = token;
							token.SetString(x.global->GetItemList(position[j]).Get());
							param["value"] = token;
							token.SetBool(false);
							param["is_user_type"] = token;

							std::string result = vm->Run(event_id, global, param)[0].ToString();

							x.global->GetItemList(position[j]).Set(0, result);

						}
					}
				}
				else {
					std::string name = x.ut->GetItemList(it_count).GetName().substr(1);
					if (name._Starts_with("&"sv) && name.size() >= 2) {
						long long idx = std::stoll(name.substr(1));
						if (idx < 0 || idx >= x.global->GetItemListSize()) {
							return false;
						}
						auto value = x.ut->GetItemList(it_count).Get();
						x.global->GetItemList(idx).Set(0, value);
					}
					else {
						x.global->SetItem(std::string(x.ut->GetItemList(it_count).GetName().substr(1)),
							x.ut->GetItemList(it_count).Get());
					}
				}
			}
			else if (x.ut->IsUserTypeList(i) && !x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("@"sv)) {
				if (x.ut->GetUserTypeList(ut_count)->GetName()._Starts_with("$"sv)) {
					auto temp = x.ut->GetUserTypeList(ut_count)->GetName();
					auto name = temp.substr(1);

					if (name.empty()) {

						for (long long j = 0; j < x.global->GetUserTypeListSize(); ++j) {
							if (_InsertFunc(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count), vm)) {
								que.push(UtInfo(x.global->GetUserTypeList(j), x.ut->GetUserTypeList(ut_count)
									, x.dir + "/$ut" + std::to_string(j)
								));
							}
						}
					}
					else {

						auto usertype = x.global->GetUserTypeItemIdx(name);

						for (long long j = 0; j < usertype.size(); ++j) {
							if (_InsertFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), vm)) {
								que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
									x.dir + "/$ut" + std::to_string(usertype[j])));
							}
						}
					}
				}
				else {
					auto usertype = x.global->GetUserTypeItemIdx(x.ut->GetUserTypeList(ut_count)->GetName());

					for (long long j = 0; j < usertype.size(); ++j) {
						//if (_UpdateFunc(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count), eventUT)) {
						que.push(UtInfo(x.global->GetUserTypeList(usertype[j]), x.ut->GetUserTypeList(ut_count),
							x.dir + "/$ut" + std::to_string(usertype[j])));
						//	}
					}
				}
			}

			if (x.ut->IsItemList(i)) {
				it_count++;
			}
			else {
				ut_count++;
			}
			count++;
		}
	}

	return true;
}