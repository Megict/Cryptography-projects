#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <ctime>
#define step 100
#define TRACE false
#define ADV false
 

void generate(int size, std::fstream& fl);

void generate(int size, std::fstream& fl) {
    //генераци€ текста из случайных символов
    if (TRACE) printf("genration started\n");
    for (int i = 0; i < size; ++i) {
        if (TRACE && i % step == 0) {
            printf("  generated %d symboles\n", i);
        }
        char l = rand() % 27;
        if (l < 26) {
            fl << char('a' + l);
        }
        else {
            fl << ' ';
        }
    }
    if (TRACE) printf("genration finished\n");
}

double compare(std::fstream& lhs, std::fstream& rhs) {
    if (TRACE) printf("comparation started\n");
    if (!lhs.is_open() || !rhs.is_open()) {
        //if (TRACE) 
        printf("comparation failed: files not open\n");
        return NAN;
    }

    lhs.seekg(0);
    rhs.seekg(0);

    char lhsc = lhs.peek();
    char rhsc = rhs.peek();
    int cnt = 0;
    int eql = 0;
    while (lhsc != EOF && rhsc != EOF) {
        ++cnt;
        if (TRACE && cnt % step == 0) {
            if (TRACE) printf("  compared %d symboles\n", cnt);
        }
        if (lhsc == rhsc) {
            ++eql;
        }
        lhs.get();
        rhs.get();

        lhsc = lhs.peek();
        rhsc = rhs.peek();
    }
    if (TRACE) printf("comparation finished\n");
    return double(eql) / double(cnt);
}

int split(std::fstream& File, int n) { //разделить текстовый файл на n файлов равной длины
    if (TRACE) printf("split started\n");
    if (!File.is_open()) {
        if (TRACE) printf("file not open\n");
        return 0;
    }

    File.seekg(0);
    char pc = File.peek();
    int filesize = 0;
    while (pc != EOF) {
        ++filesize;
        File.get();
        pc = File.peek();
    }

    int partSize = filesize / n;
    std::string filename("splFile");

    File.seekg(0);
    pc = File.peek();
    for (int i = 0; i < n; ++i) {
        if (TRACE) printf("file %d\n", i);

        std::string curFN = filename + std::to_string(i);
        std::fstream fileOut(curFN, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

        if (TRACE) printf("starting fill %d\n", i);
        for (int j = 0; j < partSize; ++j) { //записываем только те буквы, которые могут сгенерироватьс€ в случайной полседовательности 

            if ('A' <= pc && pc <= 'Z') {
                pc -= ('A' - 'a');
            }


            if ('a' <= pc && pc <= 'z' ||
                ' ' == pc) {
                fileOut << pc;
            }

            File.get();
            pc = File.peek();
            if (pc == EOF) {
                return partSize;
            }
        }

        fileOut.close();
    }

    return partSize;

}


class WordDict {
    std::vector<std::string> dict;

public:

    WordDict() {
        dict = std::vector<std::string>(0);
    }

    void dictf() {
        for (std::string a : dict) {
            std::cout << a << "\n";
        }
    }

    bool fill(std::fstream& inFile) {
        if (TRACE) printf("fill started\n");

        if (!inFile.is_open()) {
            if (TRACE) printf("fill failed: file not open\n");
            return 0;
        }

        inFile.seekg(0);
        char fc = inFile.peek();

        std::string curWord = std::string();

        while (fc != EOF) {
            if (fc != ' ' && fc != '\n' && fc != '\t') {
                if ('A' <= fc && fc <= 'Z') {
                    fc -= ('A' - 'a');
                }


                if ('a' <= fc && fc <= 'z' ||
                    ' ' == fc) {
                    curWord.append(&fc, 1);
                }

            }
            else {
                dict.push_back(curWord);
                curWord.clear();
            }

            inFile.get();

            fc = inFile.peek();
        }
        return true;

        if (TRACE) printf("fill finished\n");
    }

    bool randomSequence(std::fstream& outFile, int size) {

        if (TRACE) printf("genration started\n");
        for (int i = 0; i < size; ++i) {
            if (TRACE && i % step == 0) {
                printf("  generated %d words\n", i);
            }
            int wordpos = rand() % dict.size();

            outFile << dict[wordpos];
            outFile << " ";

        }
        if (TRACE) printf("genration finished\n");

        return false;
    }

    bool randomSequenceNR(std::fstream& outFile, int size) {
        std::map<std::string, int> usedWords;

        if (TRACE) printf("genration started\n");
        for (int i = 0; i < size; ++i) {
            if (TRACE && i % step == 0) {
                printf("  generated %d words\n", i);
            }

            int wordpos = rand() % dict.size();
            for (int j = 0; j < 100; ++j) {

                auto fnd = usedWords.find(dict[wordpos]);

                if (usedWords.end() == fnd) {
                    break;
                }

                if (TRACE) printf("word already in text. retry\n");
                wordpos = rand() % dict.size();

                if (j == 99) {
                    if (TRACE) printf("did not find free word\n");
                    return false;
                }

            }

            usedWords.insert(std::pair<std::string, int>(dict[wordpos], wordpos));

            outFile << dict[wordpos];
            outFile << " ";

        }
        if (TRACE) printf("genration finished\n");

        return false;
    }
};


int main() {
    srand(time(NULL));

    std::fstream myFile1;
    std::fstream myFile2;
    double cmpRes;

    //заполн€ем словарь

    myFile1 = std::fstream("text.txt", std::ios_base::in);

    WordDict myDict;
    myDict.fill(myFile1);

    myFile1.close();

    myFile1 = std::fstream("text1.txt", std::ios_base::in);

    WordDict myDict2;
    myDict2.fill(myFile1);

    myFile1.close();


    //разбиваем файл на много файликов

    int sepsnt = 300;
    myFile2 = std::fstream("text2.txt", std::ios_base::in);

    int tl1 = split(myFile2, sepsnt);
    myFile2.close();

    myFile2 = std::fstream("text1.txt", std::ios_base::in);

    int tl2 = split(myFile2, sepsnt/2); //чтобы перва€ половина текстиков была из первого текста, втора€ - из второго
    myFile2.close();

    int unitLen = std::min(tl1, tl2);

    //тесты

    double compCumRes = 0;
    double pstComRes = 0;

    printf("\n\n compare  text - text\n");

    compCumRes = 0;
    double lc = 0;

    for (int i = 0; i < sepsnt/2; ++i) {
        myFile1 = std::fstream(std::string("splFile") + std::to_string(i), std::ios_base::in);
        myFile2 = std::fstream(std::string("splFile") + std::to_string(sepsnt - i - 1), std::ios_base::in);

        cmpRes = compare(myFile1, myFile2);
        if (TRACE) printf("compared %d %d: %e\n", i, sepsnt - i - 1, cmpRes);

        pstComRes = compCumRes;
        compCumRes += cmpRes;
        if (TRACE) printf("\t\t ovl res: %e\n", compCumRes/(i+1));
        if (ADV) printf("\t\t\t\t Odif: %e\n", (compCumRes - pstComRes) / (i + 1));
        lc = (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2)));

        myFile1.close();
        myFile2.close();


    }

    double oDifLast =  (compCumRes - pstComRes) / (sepsnt / 2);
    printf("\t\t fin res: %e\n", compCumRes / (sepsnt/2));

    printf("\t\t last change = %.4lf %% \n", 100*(((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2))) );


    printf("\n\n compare  random - text\n");

    compCumRes = 0;

    for (int i = 0; i < sepsnt / 2; ++i) {
        myFile1 = std::fstream(std::string("splFile") + std::to_string(i), std::ios_base::in);
        myFile2 = std::fstream("fileA.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        generate(4000, myFile2);

        cmpRes = compare(myFile1, myFile2);
        if (TRACE) printf("compared %d %d: %e\n", i, sepsnt - i - 1, cmpRes);

        pstComRes = compCumRes;
        compCumRes += cmpRes;
        if (TRACE) printf("\t\t ovl res: %e\n", compCumRes / (i + 1));
        if (ADV) printf("\t\t\t\t Odif: %e\n", (compCumRes - pstComRes) / (i + 1));
        lc = (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2)));

        myFile1.close();
        myFile2.close();


    }
    oDifLast = (compCumRes - pstComRes) / (sepsnt / 2);
    printf("\t\t fin res: %e\n", compCumRes / (sepsnt / 2));

    printf("\t\t last change = %.4lf %% \n", 100 * (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2))));

    printf("\n\n compare  random - random\n");

    compCumRes = 0;

    for (int i = 0; i < sepsnt / 2; ++i) {
        myFile1 = std::fstream("fileA.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        generate(4000, myFile1);
        myFile2 = std::fstream("fileB.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        generate(4000, myFile2);

        cmpRes = compare(myFile1, myFile2);
        if (TRACE) printf("compared %d %d: %e\n", i, sepsnt - i - 1, cmpRes);

        pstComRes = compCumRes;
        compCumRes += cmpRes;
        if (TRACE) printf("\t\t ovl res: %e\n", compCumRes / (i + 1));
        if (ADV) printf("\t\t\t\t Odif: %e\n", (compCumRes - pstComRes) / (i + 1));
        lc = (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2)));

        myFile1.close();
        myFile2.close();

    }
    oDifLast = (compCumRes - pstComRes) / (sepsnt / 2);
    printf("\t\t fin res: %e\n", compCumRes / (sepsnt / 2));

    printf("\t\t last change = %.4lf %% \n", 100 * (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2))));

    return 0;
    printf("\n\n compare  random words - text\n");

    compCumRes = 0;

    for (int i = 0; i < sepsnt / 2; ++i) {
        myFile1 = std::fstream(std::string("splFile") + std::to_string(i), std::ios_base::in);
        myFile2 = std::fstream("fileB.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        myDict.randomSequence(myFile2, 300);

        cmpRes = compare(myFile1, myFile2);
        if (TRACE) printf("compared %d %d: %e\n", i, sepsnt - i - 1, cmpRes);

        pstComRes = compCumRes;
        compCumRes += cmpRes;
        if (TRACE) printf("\t\t ovl res: %e\n", compCumRes / (i + 1));
        if (ADV) printf("\t\t\t\t Odif: %e\n", (compCumRes - pstComRes) / (i + 1));
        lc = (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2)));

        myFile1.close();
        myFile2.close();

    }
    oDifLast = (compCumRes - pstComRes) / (sepsnt / 2);
    printf("\t\t fin res: %e\n", compCumRes / (sepsnt / 2));

    printf("\t\t last change = %.4lf %% \n", 100 * (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2))));

    printf("\n\n compare  random words - random words\n");

    compCumRes = 0;

    for (int i = 0; i < sepsnt / 2; ++i) {
        myFile1 = std::fstream("fileA.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        myDict.randomSequence(myFile1, 300);
        myFile2 = std::fstream("fileB.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        myDict2.randomSequence(myFile2, 300);

        cmpRes = compare(myFile1, myFile2);
        if (TRACE) printf("compared %d %d: %e\n", i, sepsnt - i - 1, cmpRes);

        pstComRes = compCumRes;
        compCumRes += cmpRes;
        if (TRACE) printf("\t\t ovl res: %e\n", compCumRes / (i + 1));
        if (ADV) printf("\t\t\t\t Odif: %e\n", (compCumRes - pstComRes) / (i + 1));
        lc = (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2)));

        myFile1.close();
        myFile2.close();

    }
    oDifLast = (compCumRes - pstComRes) / (sepsnt / 2);
    printf("\t\t fin res: %e\n", compCumRes / (sepsnt / 2));

    printf("\t\t last change = %.4lf %% \n", 100 * (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2))));


    printf("\n\n compare  random words - random words NR\n");

    compCumRes = 0;

    for (int i = 0; i < sepsnt / 2; ++i) {
        myFile1 = std::fstream("fileA.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        myDict.randomSequence(myFile1, 300);
        myFile2 = std::fstream("fileB.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        myDict.randomSequenceNR(myFile2, 300);

        cmpRes = compare(myFile1, myFile2);
        if (TRACE) printf("compared %d %d: %e\n", i, sepsnt - i - 1, cmpRes);

        pstComRes = compCumRes;
        compCumRes += cmpRes;
        if (TRACE) printf("\t\t ovl res: %e\n", compCumRes / (i + 1));
        if (ADV) printf("\t\t\t\t Odif: %e\n", (compCumRes - pstComRes) / (i + 1));
        lc = (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2)));

        myFile1.close();
        myFile2.close();

    }
    oDifLast = (compCumRes - pstComRes) / (sepsnt / 2);
    printf("\t\t fin res: %e\n", compCumRes / (sepsnt / 2));

    printf("\t\t last change = %.4lf %% \n", 100 * (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2))));

    printf("\n\n compare  random wordsNR - text\n");

    compCumRes = 0;

    for (int i = 0; i < sepsnt / 2; ++i) {
        myFile1 = std::fstream(std::string("splFile") + std::to_string(i), std::ios_base::in);
        myFile2 = std::fstream("fileB.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        myDict.randomSequenceNR(myFile2, 300);

        cmpRes = compare(myFile1, myFile2);
        if (TRACE) printf("compared %d %d: %e\n", i, sepsnt - i - 1, cmpRes);

        pstComRes = compCumRes;
        compCumRes += cmpRes;
        if (TRACE) printf("\t\t ovl res: %e\n", compCumRes / (i + 1));
        if (ADV) printf("\t\t\t\t Odif: %e\n", (compCumRes - pstComRes) / (i + 1));
        lc = (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2)));

        myFile1.close();
        myFile2.close();

    }
    oDifLast = (compCumRes - pstComRes) / (sepsnt / 2);
    printf("\t\t fin res: %e\n", compCumRes / (sepsnt / 2));

    printf("\t\t last change = %.4lf %% \n", 100 * (((compCumRes - pstComRes) / (sepsnt / 2)) / (compCumRes / (sepsnt / 2))));

}

/*
void generate(int size, std::fstream& fl) {
    //генераци€ текста из случайных символов (с частотой как в английских текстах)
    if (TRACE) printf("genration started\n");
    for (int i = 0; i < size; ++i) {
        if (TRACE && i % step == 0) {
            printf("  generated %d symboles\n", i);
        }

        int l = rand() % 1000;
        if (l < 64) {
            fl << char('a');
            continue;
        }
        l -= 64;
        if (l < 14) {
            fl << char('b');
            continue;
        }
        l -= 14;
        if (l < 27) {
            fl << char('c');
            continue;
        }
        l -= 27;
        if (l < 35) {
            fl << char('d');
            continue;
        }
        l -= 35;
        if (l < 100) {
            fl << char('e');
            continue;
        }
        l -= 100;
        if (l < 20) {
            fl << char('f');
            continue;
        }
        l -= 20;
        if (l < 14) {
            fl << char('g');
            continue;
        }
        l -= 14;
        if (l < 42) {
            fl << char('h');
            continue;
        }
        l -= 42;
        if (l < 63) {
            fl << char('i');
            continue;
        }
        l -= 63;
        if (l < 3) {
            fl << char('j');
            continue;
        }
        l -= 3;
        if (l < 6) {
            fl << char('k');
            continue;
        }
        l -= 6;
        if (l < 35) {
            fl << char('l');
            continue;
        }
        l -= 35;
        if (l < 20) {
            fl << char('m');
            continue;
        }
        l -= 20;
        if (l < 56) {
            fl << char('n');
            continue;
        }
        l -= 56;
        if (l < 56) {
            fl << char('o');
            continue;
        }
        l -= 56;
        if (l < 17) {
            fl << char('p');
            continue;
        }
        l -= 17;
        if (l < 4) {
            fl << char('q');
            continue;
        }
        l -= 4;
        if (l < 49) {
            fl << char('r');
            continue;
        }
        l -= 49;
        if (l < 56) {
            fl << char('s');
            continue;
        }
        l -= 56;
        if (l < 71) {
            fl << char('t');
            continue;
        }
        l -= 71;
        if (l < 31) {
            fl << char('u');
            continue;
        }
        l -= 31;
        if (l < 10) {
            fl << char('v');
            continue;
        }
        l -= 10;
        if (l < 18) {
            fl << char('w');
            continue;
        }
        l -= 18;
        if (l < 3) {
            fl << char('x');
            continue;
        }
        l -= 3;
        if (l < 18) {
            fl << char('y');
            continue;
        }
        l -= 18;
        if (l < 2) {
            fl << char('z');
            continue;
        }
        l -= 2;
        fl << ' ';
    }
    if (TRACE) printf("genration finished\n");
}
//
//A	0, 064	N	0, 056
//B	0, 014	O	0, 056
//C	0, 027	P	0, 017
//D	0, 035	Q	0, 004
//E	0, 100	R	0, 049
//F	0, 020	S	0, 056
//G	0, 014	T	0, 071
//H	0, 042	U	0, 031
//I	0, 063	V	0, 010
//J	0, 003	W	0, 018
//K	0, 006	X	0, 003
//L	0, 035	Y	0, 018
//M	0, 020	Z	0, 002
//ѕробел / пунктуаци€	0, 166
*/