

#pragma once


#include "node.h"


namespace taul {


    class traverser {
    public:

        traverser() = default;

        virtual ~traverser() noexcept = default;


        // traverse uses the traverser to perform a depth-first traversal of nd

        void traverse(const node& nd);


    protected:

        virtual void on_begin();
        virtual void on_end();

        // during traversal, skip_children may be set to true to tell taul::traverse
        // to skip traversing the children of the node in question

        virtual void on_enter(const node& nd, bool& skip_children) = 0;
        virtual void on_exit(const taul::node& nd) = 0;


    private:
        
        void _traverse(const node& nd);
    };
}

