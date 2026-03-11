class Solution {
    const string vowel = "aeiouAEIOU";
public:
    string reverseVowels(string s) {
        vector<int> pos;
        for(int i = 0; i < s.size(); ++i){
            if(vowel.find(s[i]) != string::npos) pos.emplace_back(i);
        }
        
        int left = 0, right = pos.size() - 1;
        while(left < right){
            swap(s[pos[left++]], s[pos[right--]]);
        }
        return s;
    }
};
