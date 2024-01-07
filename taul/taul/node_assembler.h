

#pragma once


#include "logger.h"
#include "node.h"


namespace taul {


    // node_assembler is used to simplify the process of building complex
    // node trees, which otherwise can be quite *painful* to assemble

    // node_assembler operates via a notion of 'scope' in which the
    // assembler at any given time has a particular node as its current
    // target, onto which it attaches new nodes

    // these scopes are organized into a stack structure, w/ the assembler
    // being able to be instructed to *enter* the scope of a child node
    // of the currently scoped-to node, after which it may then be instructed
    // to *exit* from said new scope, returning to said prior node


    class node_assembler final {
    public:

        node_assembler();
        node_assembler(const std::shared_ptr<logger>& lgr);

        node_assembler(const node_assembler&) = delete;
        node_assembler(node_assembler&&) noexcept = delete;

        ~node_assembler() noexcept = default;

        node_assembler& operator=(const node_assembler&) = delete;
        node_assembler& operator=(node_assembler&&) noexcept = delete;


        // TODO: the below undefined behaviour is purely due to my lazyness, so
        //       maybe add more proper semantics for them later on, w/ unit tests

        // begin is called to start a new node assembly process

        // begin behaviour is undefined if already in a node assembly process

        node_assembler& begin(node&& x);

        // end is called to finish the current node assembly process, returning
        // the assembled node tree, and resetting the node assembler's state

        // end behaviour is undefined if not currently in a node assembly process

        node end();

        // attach attaches a node to the end of the child node sequence of the 
        // node currently in scope, w/out entering into the attached node's scope

        // attach behaviour is undefined if not in the scope of a node

        node_assembler& attach(node&& x);

        // enter attaches a node to the end of the child node sequence 
        // of the node currently in scope, entering into the attached node's scope

        // enter behaviour is undefined if not in the scope of a node

        node_assembler& enter(node&& x);

        // exit exits the scope of the node currently in scope

        // exit behaviour is undefined if it is used to attempt to exit the
        // scope of the original node which started the node assembly process

        node_assembler& exit();


    private:

        std::vector<node> _scope_stack;


        void _push(node&& x);
        node _pop();
        void _attach(node&& x);
    };
}

