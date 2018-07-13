/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2016-2017.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 * 
 *  contact@voidware.com
 */

#pragma once

#include <iostream>
#include "utils.h"

template<typename ID> class Graph
{
    // directed graph of nodes and connections

 public:

    Graph() {}

    virtual ~Graph() { clear(); }

    struct Node
    {
        Node() { _mark = false; }
        Node(const ID& id) : _id(id) { _mark = false; }
        
        virtual ~Node() {}

        friend std::ostream& operator<<(std::ostream& os, const Node& n)
        { return os << n._id; }

        ID              _id;
        mutable bool    _mark;
    };

    struct Connection
    {
        Connection() { _from = 0; _to = 0; }
        Connection(Node* a, Node* b) : _from(a), _to(b) { _bi = false; }

        virtual ~Connection() {}

        friend std::ostream& operator<<(std::ostream& os, const Connection& c)
        { return os << *c._from << " -> " << *c._to; }

        Node*   _from;
        Node*   _to;
        bool    _bi;
    };

    typedef std::vector<Node*>         nodesT;
    typedef std::vector<Connection*>   connectionsT;

    const nodesT&       nodes() const { return _nodes; }
    const connectionsT& connections() const { return _connections; }
    Node*               at(int i) const { return _nodes[i]; }
    size_t              size() const { return _nodes.size(); }
    size_t              csize() const { return _connections.size(); }
    Connection*         con(int i) const { return _connections[i]; }
    

    Node* findNode(const ID& id) const
    {
        for (size_t i = 0; i < _nodes.size(); ++i)
        {
            Node* n = _nodes[i];
            if (n->_id == id) return n;
        }
        return 0;
    }

    Connection* findConnection(Node* na, Node* nb) const
    {
        if (na && nb)
        {
            for (size_t i = 0; i < _connections.size(); ++i)
            {
                Connection* ci = _connections[i];
                if ((ci->_from == na && ci->_to == nb)
                    || (ci->_bi && ci->_from == nb && ci->_to == na))
                    return ci;
            }
        }
        return 0;
    }

    Connection* findConnection(const ID& a, const ID& b) const
    {
        return findConnection(findNode(a), findNode(b));
    }

    // Features
    void add(Node* n)
    {
        // we now own the node
        _nodes.push_back(n);
    }
    
    void add(Connection* c)
    {
        // we now own the connection
        _connections.push_back(c);
    }

    int removeConnections(Node* n)
    {
        // remove any connections to or from this node
        int nr = 0;
        
        for (typename connectionsT::iterator it = _connections.begin();
             it != _connections.end();)
        {
            Connection* c = *it;
            if (c->_from == n || c->_to == n)
            {
                //onRemove(c._from, c._to);
                it = _connections.erase(it);
                ++nr;

                delete c;
            }
            else
                ++it;
        }
        return nr;
    }

    int removeConnections(Node* a, Node* b)
    {
        // remove connections a->b or *->b if a == 0 or a->* if b==0
        int nr = 0;
        for (typename connectionsT::iterator it = _connections.begin();
             it != _connections.end();)
        {
            Connection* c = *it;
            if ((!a || c->_from == a) && (!b || c->_to == b))
            {
                it = _connections.erase(it);
                ++nr;

                delete c;
            }
            else ++it;
        }
        return nr;
    }

    void remove(Node* n)
    {
        if (n)
        {
            // remove any connections to or from this node
            removeConnections(n);

            // and delete the node
            erase(_nodes, n);

            delete n;
        }
    }

    void remove(Connection* c)
    {
        if (c)
        {
            erase(_connections, c);
            delete c;
        }
    }

    void markNodesFrom(Node* a) const
    {
        // mark depth first
        if (!a->_mark)
        { 
            a->_mark = true;
            for (size_t i = 0; i < _connections.size(); ++i)
            {
                Connection* c = _connections[i];
                if (c->_from == a) markNodesFrom(c->_to);
            }            
        }
    }

    bool isConnected(Node* a, Node* b) const
    {
        // is `b` reachable from `a`
        _clearMarks();

        markNodesFrom(a);

        return b->_mark;
    }

    struct Link
    {
        Link(Node* n, Link* next) : _node(n), _next(next) {}
        
        Node*   _node;
        Link*   _next;
    };

    typedef std::list<Link> Links;
    typedef std::vector<Node*> Path;

    bool shortestPath(Node* a, Node* b, Path& path) const
    {
        if (!a || !b) return false;  // bail
        
        bool res = false;
        _clearMarks();

        Links links;
        links.emplace_back(Link(a, 0));
        a->_mark = true;

        for (typename Links::iterator it = links.begin();
             it != links.end(); ++it)
        {
            Link* li = &*it;
            if (li->_node == b)
            {
                // found target

                // reverse links in chain
                Link* last = 0;
                do
                {
                    Link* ln = li->_next;
                    li->_next = last;
                    last = li;
                    li = ln;
                } while (li);

                // walk forward pushing result
                do
                {
                    path.push_back(last->_node);
                    last = last->_next;
                } while (last);

                res = true;
                break;
            }
            else
            {
                for (size_t i = 0; i < _connections.size(); ++i)
                {
                    Connection* c = _connections[i];
                    Node* to = 0;
                    if (c->_from == li->_node && !c->_to->_mark)
                        to = c->_to;
                    else if (c->_bi && c->_to == li->_node && !c->_from->_mark)
                        to = c->_from;
                    
                    if (to)
                    {
                        links.emplace_back(Link(to, li));
                        to->_mark = true;
                    }
                }                    
            }
        }
        return res;
    }

#if 0
    bool addNonCyclic(Node* a, Node* b)
    {
        bool res = false;
        if (a && b)
        {
            // if b can reach a, then dont allow
            if (!isConnected(b, a))
            {
                add(a, b);
                res = true;
            }
        }
        return res;
    }
#endif

    void clearAllConnections()
    {
        purge(_connections);
    }

    void clear()
    {
        clearAllConnections();
        purge(_nodes);
    }

protected:

    nodesT              _nodes;
    connectionsT        _connections;

private:

    void _clearMarks() const
    {
        for (size_t i = 0; i < _nodes.size(); ++i) _nodes[i]->_mark = false;
    }
};
