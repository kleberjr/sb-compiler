#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <bits/stdc++.h>

using namespace std;

vector<string> readAllLines(const string& path) {
	ifstream in(path);

	if (!in.is_open()) {
		throw runtime_error("Erro ao abrir arquivo: " + path);
	}

	vector<string> lines;
	string line;

	while (getline(in, line)) {
		lines.push_back(line);	
	}

	return lines;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cerr << "Erro: número inesperado de argumentos\n";
		return 1;
	}	

	string inputPath = argv[1];
 
	try {
		auto lines = readAllLines(inputPath);

		cout << "Arquivo: " << inputPath << "\n";
		cout << "Número de linhas: " << lines.size() << "\n";

		cout << "Primeiras linhas:\n";

		size_t limit = min<size_t>(20, lines.size());

		for (size_t i = 0; i < limit; ++i) {
			cout << (i + 1) << ": " << lines[i] << "\n";
		}
	} catch (const exception& ex) {
		cerr << "Erro: " << ex.what() << "\n";
		return 1;
	}

	return 0;
}
