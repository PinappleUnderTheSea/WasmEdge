//
// Created by junjie.zhu on 24-4-28.
//

#include<iostream>

using namespace std;

int main(int argc, char *argv[]){
    string string1 = string(argv[1]);
    string string2 = string(argv[2]);

  auto start = std::chrono::high_resolution_clock::now();


  // 获取结束时间点
    for(int i=0; i<100000000; i++){
      // cout << string1.find(string2) << endl;
      string1.find(string2);
    }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

  cout << duration.count() << endl;
    return 0;
}