#include <cstddef>
#include <cstring>
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
#define STACK_ERROR     1
#define INVALID_COOKIE  2

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
        std::cerr << "Cannot Push To Stack - Stack Full" << std::endl;
        throw STACK_ERROR;
    }
    return;
}

char pop(stack *my_stack) {
    if (my_stack->index > 0) {
        char value = my_stack->array[my_stack->index];
        my_stack->index--;
        return value;
    } else {
        std::cerr << "Cannot Pop From Stack - Stack Empty" << std::endl;
        throw STACK_ERROR;
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
        std::cerr << "No Cooking Provided" <<std::endl;
        throw INVALID_COOKIE;
    }

    user_dict[current_key] = current_value;

    return user_dict;
}

int main(void) {
    std::string cookie = "foo=bar&";
    std::map<std::string, std::string> parsed = parse_cookie(cookie);

    auto my_iter = parsed.begin();
    while (my_iter != parsed.end()) {
        std::cout << "Key: " << my_iter->first << '\n';
        std::cout << "Value: " << my_iter->second << '\n';
        my_iter++;
    }

    return 0;
}
