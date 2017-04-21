#include "flow/flow.hpp"
#include <string>
#include <iostream>
#include "block/logger.hpp"
//#include "block/websocket.hpp"
using namespace std;

//flow::Edge<int> edgeC;

class A
{
private:
    flow::Edge<int>& m_edge;
public:
    A(flow::Edge<int>& out_edge) : m_edge(out_edge),i(0) {
    }
    void operator() (flow::inputs& in, flow::outputs& out) {
        this_thread::sleep_for(chrono::milliseconds(500)); // 1000
        //out(edgeC.id) = i++;
        out(m_edge.id) = i++;
        if (i>10) {
            std::cout << "** stop_session() **" << std::endl;
	    	flow::stop_session();
	    }
    }
private:
    int i;
};

class B
{
private:
    flow::Edge<int>& m_edge;
public:
    B(flow::Edge<int>& in_edge) : m_edge(in_edge) {
    }
    void operator() (flow::inputs& in, flow::outputs& out) {
        //this_thread::sleep_for(chrono::milliseconds(1));
        //auto x = in(edgeC.id).cast<decltype(edgeC.value)>();
        auto x = in(m_edge.id).cast<int>();
        cout << "x=" << x << endl;
    }
};

class AB
{
private:
    flow::Edge<int>& m_in_edge;
    flow::Edge<int>& m_out_edge;
public:
    AB(flow::Edge<int>& in, flow::Edge<int>& out) : m_in_edge(in), m_out_edge(out)
    {
    }
    void operator() (flow::inputs& in, flow::outputs& out) {
        auto x = in(m_in_edge.id).cast<int>();
        out(m_out_edge.id) = x*2;
    }
};

int main(int argc, char* argv[])
{   //
    flow::Edge<int> edgeC;
    flow::Edge<int> edgeD;

    flow::Logger log("log.txt");

    A a(edgeC);
    auto closureA = flow::create_closure(a);
    flow::FuncBlock blockA(closureA,false,true); // 第二引数はメインスレッド固定で実行を行うかどうか(true:固定,false:マルチスレッド)
    // 第三引数はジェネレータ（非リアクティブ）に動作させるかどうか(true:poll毎に呼ばれる、false:イベントが前のブロックで生じたら呼ばれる)

    B b(edgeD); //edgeC
    auto closureB = flow::create_closure(b);
    flow::FuncBlock blockB(closureB);

    AB ab(edgeC, edgeD);
    auto closureAB = flow::create_closure(ab);
    flow::FuncBlock blockAB(closureAB);

    edgeC.from() = blockA;
    edgeC.to() = blockAB;
    edgeD.from() = blockAB;
    edgeD.to()   = blockB;

    cout << "last edgeD.id=" << edgeD.id << endl;
    cout << "last edgeC.id=" << edgeC.id << endl;

    flow::start_session();

}
