// SelenaClientFastCom.cpp
// Walter Artyomenko, August 2019
//
#include "SelenaClient.h"

//-----------------------------------------------------------------------------
int ExecutFastCommand(ZSelenaMessage& M, ZSelenaSocket* pClient)
{
	//---------------------------------------------------------------------
	// Проверка команды на запрос списка или колличества 
	// запланированных работ
	if (IsJobCountCommand(M.msg)) {
		std::string s = "Job count: ";
		int count = clientJobsStorage->V().GetJobsCount();

		if (count > 0) {
			char istr[32];
			sprintf_s(istr, 32, "%d", count);
			s += istr;
			s += ".";
		}
		else s = "Job list is empty.";

		MessageToServer(s, "", smtText);
		return 0;
	}

	if (IsJobListCommand(M.msg)) {
		// Get job neme list
		std::vector<std::string> JL;
		clientJobsStorage->V().GetJobsList(JL);
		// Build ansver string for server.
		std::string sl("Job list:\n");
		int i, n = JL.size();
		if (n) {
			for (i = 0; i < n; ++i) {
				// job
				SelenaJob J;
				J.storageName = JL[i];
				// Read job command
				if (clientJobsStorage->V().RestoryJ(J)) {
					continue;
				}
				// Add job command to ansver string.
				char sn[32];
				snprintf(sn, 32, "%d) ", i + 1);
				sl += sn + J.storageName + ": " + J.command;
				if (i < n - 1) sl += "\n";
			}
		}
		else {
			sl = "Job list: is empty.";
		}
		MessageToServer(sl, "", smtText);

		return 0;
	}

	//---------------------------------------------------------------------
	// Проверка команды на удаление одной или нескольких из 
	// запланированных работ
	if (IsJobRemoveCommand(M.msg)) {
		// Cut off first 11 char from command
		std::vector<int> I;
		std::string m = M.msg.substr(11);

		if (CommandFindWord(m, "all") < 0) {
			std::vector<int> n;
			std::string      v;
			char *s = (char*)m.c_str();
			int   j = 0, j0 = 0, k;
			char  isRange = 0;

			for (int i = 0; 1; i++) {
				char c = s[i];
				if (c >= '0' && c <= '9') {
					if (!j) j0 = j = i;
					j++;
				} else {
					if (j > j0) {
						// целое число расположено в строке от j0 до j.
						v = m.substr(j0, j - j0);
						k = std::stoi(v);
						int count = (int)n.size();
						if (count && isRange) {
							// задан диапазон значений.
							int nk = n[count - 1];
							if (nk <= k) 
								{ while (++nk <= k) n.push_back(nk); }
							else
								{ while (k < nk) { n.push_back(k); k++; } }
						}
						else n.push_back(k);
						j = 0; j0 = 0;
					} 
					if (isRange >= 0) {
						if (c != ' ' && c != '-') {
							isRange = -1;
							// если не корректные символы,
							// то прекращаем анализ строки
							if (c != ',' && c != '.' &&
								c != ' ' && c != '-') break;
						} else {
							if (c == '-') isRange = 1;
				}	}	}
				if (!s[i]) break;
			}
			// Удаление запланированных работ
			m = "No jobs removed";
			if (n.size()) {
				sort(n.begin(), n.end());
				int count = clientCom->V().Remove(&n);
				if (count) {
					m = "Remove ";
					m += std::to_string(count);
					m += count == 1 ? " job" : " jobs";
			}	}
		} else {
			// Удаление всех запланированных работ
			clientCom->V().Remove(nullptr);
			m = "All scheduled jobs are deleted.";
		}
		MessageToServer(m);
		return 0;
	}

	return -1;
}