#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <functional>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <limits>
#include <ctype.h>

#define LOOP_CNT 5

using namespace std;

//@todo 構造体として命名させておくと、変数が何であったか分かる
typedef vector<pair<string, vector<string> > > Feats; // タグ, 特徴ベクトル
typedef map<string, map<string, double> > Weight; // タグ、重みベクトル

void log(string str);
vector<string> split(const string &str, char delim);
bool readFile(ifstream &fin, Feats &feats);
void addFeat(const string word, vector<string> &feat);
bool perceptron(int times, Weight &w, Feats &feats);
string classify(Weight &w, const vector<string> &feat);
void test(Feats &feats, Weight &w);


void log(string str){
	cout << str << " end...\n";
}

/*
 指定された文字で分割、vector形式で結果を返す
 引数：
 	 string str 入力文字列
 	 char delim 区分する文字
 戻り値：
 	 vector<string> 分割された文字列たち
 */
vector<string> split(const string &str, char delim){
	istringstream iss(str);
	string tmp;
	vector<string> res;
	while(getline(iss, tmp, delim)){
		res.push_back(tmp);
	}
	return res;
}


/*
 ファイル中の単語達を分割し、タグ->特徴ベクトル のFeatsを作成
 引数：
 	 ifstream fin 入力ファイル
 	 Feats feats
 戻り値：
 	 bool 成功かどうか
 */
bool readFile(ifstream &fin, Feats &feats){
	if(fin.fail()){
		cout << "cannot read file!\n";
		return false;
	} else {
		cout << "Succeed reading file!\n";
	}

	string buf;
	// 一文ずつ読み込む
	while(getline(fin, buf)){
		vector<string> words = split(buf, ' ');

		// １語ずつ特徴ベクトル作成
		for(int i = 0; i < (int)words.size(); i++){
			int found = words[i].find_last_of('/');
			string word = words[i].substr(0, found);
			string tag = words[i].substr(found+1);
			vector<string> feat;

			// 単語自信という特徴付与
			feat.push_back("W_"+word);

			// 着目単語の周辺単語の特徴付与
			if((i-3) >= 0){
				int found_ = words[i-3].find_last_of('/');
				string word_ = words[i-3].substr(0, found_);
				feat.push_back("W-3_"+word_);
			}
			if((i-2) >= 0){
				int found_ = words[i-2].find_last_of('/');
				string word_ = words[i-2].substr(0, found_);
				feat.push_back("W-2_"+word_);
			}
			if((i-1) >= 0){
				int found_ = words[i-1].find_last_of('/');
				string word_ = words[i-1].substr(0, found_);
				feat.push_back("W-1_"+word_);
			}
			if((i+1) < (int)words.size()){
				int found_ = words[i+1].find_last_of('/');
				string word_ = words[i+1].substr(0, found_);
				feat.push_back("W+1_"+word_);
			}
			if((i+2) < (int)words.size()){
				int found_ = words[i+2].find_last_of('/');
				string word_ = words[i+2].substr(0, found_);
				feat.push_back("W+2_"+word_);
			}

			// 着目単語の周辺単語のタグ情報付与
			if((i-1) >= 0){
				int found_ = words[i-1].find_last_of('/');
				string tag_ = words[i-1].substr(found_+1);
				feat.push_back("T-1_"+tag_);
			}
			if((i+1) < (int)words.size()){
				int found_ = words[i+1].find_last_of('/');
				string tag_ = words[i+1].substr(found_+1);
				feat.push_back("T+1_"+tag_);
			}

			// 接尾辞接頭語による特徴付与
			addFeat(word, feat);

			feats.push_back(make_pair(tag, feat));
		}
	}
	cout << "feats' size: " << feats.size() << endl;

	return true;
}

//SUFFIXリストを生成せずに、単語の先頭２文字を全部特徴として保持させる、などもあり
void addFeat(const string word, vector<string> &feat){
	vector<string> suffix, prefix;
	suffix.push_back("an");
	suffix.push_back("ar");
	suffix.push_back("al");
	suffix.push_back("age");
	suffix.push_back("ate");
	suffix.push_back("ant");
	suffix.push_back("able");
	suffix.push_back("ance");
	suffix.push_back("ancy");
	suffix.push_back("cy");
	suffix.push_back("dom");
	suffix.push_back("ence");
	suffix.push_back("er");
	suffix.push_back("ed");
	suffix.push_back("ee");
	suffix.push_back("en");
	suffix.push_back("ean");
	suffix.push_back("ess");
	suffix.push_back("ful");
	suffix.push_back("hood");
	suffix.push_back("ic");
	suffix.push_back("ly");
	suffix.push_back("ian");
	suffix.push_back("ist");
	suffix.push_back("ity");
	suffix.push_back("ism");
	suffix.push_back("ive");
	suffix.push_back("ify");
	suffix.push_back("ise");
	suffix.push_back("ize");
	suffix.push_back("ing");
	suffix.push_back("ical");
	suffix.push_back("ious");
	suffix.push_back("ised");
	suffix.push_back("ible");
	suffix.push_back("less");
	suffix.push_back("ment");
	suffix.push_back("nomy");
	suffix.push_back("ness");
	suffix.push_back("or");
	suffix.push_back("ous");
	suffix.push_back("ology");
	suffix.push_back("rr");
	suffix.push_back("s");
	suffix.push_back("sion");
	suffix.push_back("ship");
	suffix.push_back("ty");
	suffix.push_back("tion");
	suffix.push_back("ure");
	suffix.push_back("y");

	prefix.push_back("a");
	prefix.push_back("dis");
	prefix.push_back("en");
	prefix.push_back("up");
	prefix.push_back("un");


	for (int i = 0; i < (int)suffix.size(); i++) {
		if(word.length() >= suffix[i].length()){
			if( word.substr(word.size()-suffix[i].length()) == suffix[i]){
				feat.push_back("SUF_"+suffix[i]);
			}

		}
	}
	for (int i = 0; i < (int)prefix.size(); i++) {
		if(word.length() >= prefix[i].length()){
			if( word.substr(0, prefix[i].length()-1) == prefix[i]){
				feat.push_back("PRE_"+prefix[i]);
			}
		}
	}
	if(isupper(word[0])){
		feat.push_back("UPPER");
	}
}

/*
 平均化パーセプトロンで機械学習

 */
bool perceptron(int times, Weight &w, Feats &feats){
	Weight v = w;
	int nfeats = (int)feats.size();
	int loop_cnt = 0;
	Weight::iterator itr;

	for (int t = 0; t < times; t++) {
		for(int i = 0; i < nfeats; i++){
			// サンプル取得
			string ans = feats[i].first;
			const vector<string> &feat = feats[i].second;

			// 分類
			string myans = classify(w, feat);

			// もし分類が間違っていれば重みベクトルを更新
			if(ans != myans){
				for (int j = 0; j < (int)feat.size(); j++) {
					w[ans][feat[j]] += 1.0;
					w[myans][feat[j]] -= 1.0;

					// 重みベクトルの和も更新（効率化された計算）
					v[ans][feat[j]] += loop_cnt;
					v[myans][feat[j]] -= loop_cnt;
				}
			}
			// 進捗出力
			if(loop_cnt % ((times*nfeats)/100) == 0) cout << "perceptron " << loop_cnt/((times*nfeats)/100) <<"% end!\n";
			loop_cnt++;
		}
	}
	for (itr = w.begin(); itr != w.end(); itr++) {
		map<string, double>::iterator vec;
		for (vec = itr->second.begin(); vec != (itr->second.end()); vec++) {
			itr->second[vec->first] -= v[itr->first][vec->first]/(double)(loop_cnt);
		}
	}
	return true;
}


string classify(Weight &w, const vector<string> &feat){
	string str = "NN";
	double max = -numeric_limits<double>::infinity();
	Weight::iterator itr;
	for (itr = w.begin(); itr != w.end(); itr++) {
		double val = 0.0;
		for (int j = 0; j < (int)feat.size(); j++) {
			val += (itr->second.count(feat[j]) == 1) ? (double)itr->second[feat[j]] : 0.0;
		}
		if(val > max){
			str = itr->first;
			max = val;
		}
	}
	assert(str != "");
	return str;
}


/*
 テストデータを用いて検証
 */

void test(Feats &feats, Weight &w){
	int success = 0, test_cnt = 0;
	for (int i = 0; i < (int)feats.size(); i++) {
		if(feats[i].first == classify(w, feats[i].second)) success++;
		test_cnt++;
	}
	cout << "成功率: " << (double)success*100/test_cnt << "%\n";
	return;
}


int main(){
	// 準備
	srand((unsigned) time(NULL));

	ifstream learnData("./data/wsj00-18.pos"), testData("./data/wsj19-21.pos");
	Feats feats_for_learning, feats_for_test;
	Weight weight;

	// 学習ファイルの読み込み
	if(!readFile(learnData, feats_for_learning)) return -1;
	log("readFile");

	perceptron(LOOP_CNT, weight, feats_for_learning);
	log("perceptron");


	// 実際にテスト
	if(!readFile(testData, feats_for_test)) return -1;
	test(feats_for_test, weight);
	log("test");

	return 0;
}
