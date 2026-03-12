class Solution {
public:
    bool isPalindrome(string s) {
        int right = s.size() - 1, left = 0;
        
        while(right > left){
            while(right >= 0 && ! isalnum(s[right])) right--;
            while(left < s.size() && ! isalnum(s[left])) left++;
            if(right < left) return true;
            
            if(tolower(s[right]) != tolower(s[left])) return false;
            right--;
            left++;
        }
        return true;
    }
};
