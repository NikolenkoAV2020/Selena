// SelenaClientMergeFiles.cpp
// Walter Artyomenko, August 2019
//
#include "SelenaClient.h"
#include "SelenaClientGlobal.h"

//-----------------------------------------------------------------------------
static int findNextJsonContent(char** pps)
{
	char* rs = *pps;
	char  nq = 0;
	char  nz = 0;
	while (1) {
		if (*rs == '\0') { 
			*pps = rs; return -1; 
		}
		if (*rs == '"') { 
			if (nq) return 0;
			++rs; 
			++nq; 
			continue; 
		}
		if (*rs == ',') { 
			if (nz) return 0;
			++rs; 
			++nz; 
			continue; 
		}
		if (*rs == ' ') { 
			++rs; 
			continue; 
		}
		break;
	}
	*pps = rs;
	return 0;
}

//-----------------------------------------------------------------------------
static int findNextTextContent(char** pps)
{
	char* rs = *pps;
	char  nq = 0;
	while (1) {
		if (*rs == '\0') return -1;
		if (*rs == ':') {
			if (nq) return-1;
			++rs; ++nq; continue;
		}
		if (*rs == ' ') {
			++rs; continue;
		}
		break;
	}
	*pps = rs;
	return 0;
}

//-----------------------------------------------------------------------------
static int findNextSeparate(char** pps, const char sp)
{
	char* rs = *pps;
	while (1) {
		if (*rs == '\0') return -1;
		if (sp) {
			if (*rs == sp) return 0;
		} else {
			if (*rs == ':') return 0;
			if (*rs == ' ') return 0;
			if (*rs == ',') return 0;
		}
		++rs;
		*pps = rs;
	}
	return 0;
}

//-----------------------------------------------------------------------------
static int GetIPstringFromFile(
	IpAddressTrawler& T,
	std::ifstream&    ifile,
	std::string&      s,
	int&   l,
	char** ips,
	char** rs)
{
	while (1) {
		getline(ifile, s);
		l = s.length();
		if (l) {
			*ips = T((char*)s.c_str(), rs);
			if ((*ips)[0] != '\0') break;
		}
		if (ifile.eof()) return-1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
int MergeIpJsonFiles(
	// source file 1
	const std::string sourcef1,
	// source file 2
	const std::string sourcef2,
	// result file
	const std::string resultf)
{
	char b[1024];
	int  r;

	// open source file
	std::ifstream ifile1;
	ifile1.open(sourcef1, std::ios::in);
	if (!ifile1.is_open()) return 1;

	// open source file
	std::ifstream ifile2;
	ifile2.open(sourcef2, std::ios::in);
	if (!ifile2.is_open()) return 1;

	// open result file
	std::ofstream ofile;
	ofile.open(resultf, std::ios::out | std::ios::trunc);
	if (!ofile.is_open()) return 2;

	IpAddressTrawler T;
	std::string      s1, s2, s;

	ofile << "Host/port scan results. Executed by Selena client" 
		  << std::endl
		  << "[" << std::endl;

	while (1) {
		s1 = s2 = "";
		// Find ip-address in string
		char* rs1 = nullptr;
		char* ips1 = nullptr;
		char* rs2 = nullptr;
		char* ips2 = nullptr;

		int l1, l2;

		r = GetIPstringFromFile(T, ifile1, s1, l1, &ips1, &rs1);
		if (r) break;

		r = GetIPstringFromFile(T, ifile2, s2, l2, &ips2, &rs2);
		if (r) break;

		while (strcmp(ips1, ips2)) {
			r = GetIPstringFromFile(T, ifile1, s1, l1, &ips1, &rs1);
			if (r) break;
		}
		if (r) break;

		// ќдин и тот же ip адрес
		if (rs1 && rs2 && ips1 && ips2) {
			s.reserve(l1 + l2);

			if (!findNextJsonContent(&rs1)) {
				char* ps1 = (char*)s1.c_str();
				int k = (int)(rs1 - ps1);
				s = s1.substr(0, k);
				if (!findNextTextContent(&rs2)) {
					// there is host name
					char* rs21 = rs2;
					if (!findNextSeparate(&rs21, '\0')) {
						k = (int)(rs21 - rs2);
						memcpy(b, rs2, k);
						b[k] = '\0';
						s += "\"   \"host\": \"";
						s += b;
					}
					else {

					}
					// fingerprint
					char* sfp = nullptr;
					if (!findNextSeparate(&rs2, ':')) {
						// pointer to fingerprint
						sfp = ++rs2;
						// remove closed } from json item
						k = strlen(rs1);
						--k;
						bool findbrecket = false;
						while (1) {
							if ( findbrecket && rs1[k] != ' ') break;
							if (!findbrecket && rs1[k] == '}') findbrecket = true;
							--k;
							// Invalid format
							if (!k) break;
						}
						rs1[k+1] = '\0';
					} 
					s += rs1;
					if (sfp) {
						s += ",   \"fingerpr\": \"";
						s += sfp;
						s += "\" },";
					}
				}
				else {
					// host name is absent;
					s = s1;
				}
			}
			else {
				s = s1;
			}
		}
		else {
			s = s1;
		}

		// write found address to output file
		ofile << s << std::endl;
	}

	ofile << "]" << std::endl;

	// Close files
	ofile.close();
	ifile1.close();
	ifile2.close();

	return 0;
}

//-----------------------------------------------------------------------------