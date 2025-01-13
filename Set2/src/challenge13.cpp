#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <map>

/* 
foo=bar&baz=qux&zap=zazzle

Produces 

 {
  foo: 'bar',
  baz: 'qux',
  zap: 'zazzle'
}
*/

// TODO:
// Update error definitions to classes and properly implement
// try | catch block :)

#define STACK_SIZE      1024

class stack_error : public std::exception {
    public:
        stack_error(const std::string &message) : message_(message) {}
        const char* what() const throw() {return message_.c_str();}
    
    private:
        std::string message_;
};

class invalid_cookie_error : public std::exception {
    public:
        invalid_cookie_error(const std::string &message) : message_(message) {}
        const char* what() const throw() {return message_.c_str();}

    private:
        std::string message_;

};


typedef struct stack {
    int index;
    char array[STACK_SIZE];
} stack;

stack init_stack() {
    stack my_stack;
    memset(&my_stack.array, 0, STACK_SIZE);
    return my_stack;
}

void push(stack *my_stack, char value_to_push) {
    if (my_stack->index < STACK_SIZE) {
        my_stack->array[my_stack->index] = value_to_push;
    } else {
        throw stack_error("Cannot Push To Stack - Stack Full");
    }
    return;
}

char pop(stack *my_stack) {
    if (my_stack->index > 0) {
        char value = my_stack->array[my_stack->index];
        my_stack->index--;
        return value;
    } else {
        throw stack_error("Cannot Pop From Stack - Stack Empty");
    }
}

std::map<std::string,std::string> parse_cookie(std::string cookie) {
    std::map<std::string, std::string> user_dict;
    //Cookie Structure: a=b & c=d
    // If last symbol == '=' and current symbol == '&' error
    std::string current_key;
    std::string current_value;
    size_t index = 0;

    if (cookie.length() > 1) {
        while (cookie[index] != '=' && index < cookie.length()) {
            current_key += cookie[index];
            index++;
        }

        if (index < cookie.length() || cookie[index] == '=') {
            index++;
        }

        while (cookie[index] != '&' && index < cookie.length()) {
            current_value += cookie[index];
            index++;
        }
    
    } else {
        throw invalid_cookie_error("No Cookie Provided");
    }

    user_dict[current_key] = current_value;

    return user_dict;
}

int main(void) {
    try {
        std::string cookie = "foo=bar&";
        std::map<std::string, std::string> parsed = parse_cookie(cookie);
        auto my_iter = parsed.begin();
        while (my_iter != parsed.end()) {
            std::cout << "Key: " << my_iter->first << '\n';
            std::cout << "Value: " << my_iter->second << '\n';
            my_iter++;
        }
    } catch (const invalid_cookie_error& e) {
        std::cerr << "Invalid Cookie Error: " << e.what() << '\n';
    } catch (const stack_error& e) {
        std::cerr << "Stack Error: " << e.what() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "General Exception: " << e.what() << '\n';
     }

    return 0;
}
