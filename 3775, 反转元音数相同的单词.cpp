class Solution {
    bool check(char ch){
        if(ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u') return true;
        else return false;
    }
    
public:
    string reverseWords(string s) {
        int vowel = 0,  right = 0, n = s.size();
        while(right < n && ! isspace(s[right])){
            if(check(s[right])) vowel++;
            right++;
        }
        
        int left = right + 1, sum = 0;
        for(int r = left; r < n; ++r){
            
            while(r < n && ! isspace(s[r])){
                if(check(s[r])) sum++;
                r++;
            }
            
            if(sum == vowel) reverse(s.begin() + left, s.begin() + r);
            left = r+1;
            sum = 0;
        }
        
        return s;
    }
};
