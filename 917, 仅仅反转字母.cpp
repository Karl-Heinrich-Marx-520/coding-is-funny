class Solution {
public:
    string reverseOnlyLetters(string s) {
        vector<int> pos;
        int n = s.size();
        for(int i = 0; i < n; ++i){
            if(isalpha(s[i])) pos.emplace_back(i);
        }
        
        int left = 0, right = pos.size() - 1;
        while(left < right){
            swap(s[pos[right--]], s[pos[left++]]);
        }
        return s;
    }
};
