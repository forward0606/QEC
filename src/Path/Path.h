#include "../Node/Node.h"
#include "../Channel/Channel.h"
#include <functional>


class Path{
public:
    vector<reference_wrapper(Node)> nodes;
    vector<reference_wrapper(Channel)> channels;
    bool entangle();//true->entangle success
    bool swap();    //true->swapped success
	Path();
};