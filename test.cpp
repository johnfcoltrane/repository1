//#include "flow/flow.hpp"
#include <string>
#include <iostream>
//#include "block/logger.hpp"
//#include "block/websocket.hpp"
using namespace std;

class FuncBlock;

class Edge;

Edge* g_edges[2];

class Edge {
private:
	FuncBlock* m_fb_from;
	FuncBlock* m_fb_to;
	static int buf;
	static int n;
public:
	Edge() {
		id = 0;
		g_edges[n] = this;
		n++;
	}
	
	Edge(int i):id(i){}
	
	int id;
	FuncBlock& from() {
		return *m_fb_from;
	}
	FuncBlock& to() {
		return *m_fb_to;
	}
	int cast_int() {
		return buf;
	}
	void operator=(int i) {
		buf = i;
	}
};

int Edge::buf;
int Edge::n = 0;

class inputs {
private:
	int buf;
	Edge& m_o;
public:
	inputs(Edge* po):m_o(*po) {}
	Edge& operator()(int i){
		return m_o;
	}
};

class outputs {
private:
	//int buf;
	Edge &m_o;
	//Edge* m_po;
public:
	//outputs() {
		//m_po = NULL;
	//}
	outputs(Edge* po) : m_o(*po) {

	}

	Edge& operator()(int edge_id){
		//buf = i;
		return m_o;
		//return *m_po;
	}

};

class A
{
private:
    //flow::Edge<int>& m_edge;
	Edge& m_edge;
public:
	A(Edge& out_edge) : m_edge(out_edge),i(0) {
    }
    void operator() (inputs& in, outputs& out) {

        //this_thread::sleep_for(chrono::milliseconds(500)); // 1000
		out(m_edge.id) = i++; // todo:
        if (i>10) {
            std::cout << "** stop_session() **" << std::endl;
	    	//flow::stop_session();
	    }
    }
private:
    int i;
};

class B
{
private:
    Edge& m_edge;
public:
    B(Edge& in_edge) : m_edge(in_edge) {
    }
    void operator() (inputs& in, outputs& out) {
        //this_thread::sleep_for(chrono::milliseconds(1));
        //auto x = in(edgeC.id).cast<decltype(edgeC.value)>();
        auto x = in(m_edge.id).cast_int(); // todo:
        cout << "x=" << x << endl; // todo:
    }
};

class AB
{
private:
    Edge& m_in_edge;
    Edge& m_out_edge;
public:
    AB(Edge& in, Edge& out) : m_in_edge(in), m_out_edge(out)
    {
    }
    void operator() (inputs& in, outputs& out) {
        auto x = in(m_in_edge.id).cast_int(); // todo:
		out(m_out_edge.id) = x*2; // todo:
    }
};

class Closure;

class FuncBlock {
public:
	FuncBlock()
	{}
	FuncBlock(Closure& c,bool f1, bool f2){
	}
	FuncBlock(Closure& c) {

	}
};
//
class Closure {
public:
	static A* p_a;
	static AB* p_ab;
	static B* p_b;
	static Edge* p_edge_c;
	static Edge* p_edge_d;

};
A* Closure::p_a;
AB* Closure::p_ab;
B* Closure::p_b;
Edge* Closure::p_edge_c;
Edge* Closure::p_edge_d;
//
Closure g_closure;
//
Closure create_closure(A& a) {
	Closure::p_a = &a;
	return g_closure;
}
//
Closure create_closure(B& b) {
	Closure::p_b = &b;
	return g_closure;
}

Closure create_closure(AB& ab) {
	Closure::p_ab = &ab;
	return g_closure;
}
//

//Edge edgeC;
//Edge edgeD;

//Edge* g_edges[2];

void start_session() {
	inputs* ins[3];
	outputs* ous[3];
	int n = sizeof(g_edges) / sizeof(g_edges[0]);
	ins[0] = new inputs(g_edges[0]);
	for (int i = 0; i < n; i++) {
		ous[i] = new outputs(g_edges[i]);
		ins[i+1] = new inputs(g_edges[i]);
	}
	ous[n] = new outputs(g_edges[n-1]);

	//inputs in1(NULL);
	////
	//outputs out1(&edgeC);
	//inputs in2(&edgeC);
	////
	//outputs out2(&edgeD);
	//inputs in3(&edgeD);
	////
	//outputs out3(NULL);

	for (int i = 0; i < 10; i++) {
		//(*Closure::p_a)(in1, out1);
		//(*Closure::p_ab)(in2, out2);
		//(*Closure::p_b)(in3, out3);
		(*Closure::p_a)(*ins[0], *ous[0]);
		(*Closure::p_ab)(*ins[1], *ous[1]);
		(*Closure::p_b)(*ins[2], *ous[2]);
	}
}

int main(int argc, char* argv[])
{   
#if 1
	//
    Edge edgeC;
    Edge edgeD;

    //flow::Logger log("log.txt");

    A a(edgeC);
    auto closureA = create_closure(a);
    FuncBlock blockA(closureA,false,true); // 第二引数はメインスレッド固定で実行を行うかどうか(true:固定,false:マルチスレッド)
    // 第三引数はジェネレータ（非リアクティブ）に動作させるかどうか(true:poll毎に呼ばれる、false:イベントが前のブロックで生じたら呼ばれる)

    B b(edgeD); //edgeC
    auto closureB = create_closure(b);
    FuncBlock blockB(closureB);

    AB ab(edgeC, edgeD);
    auto closureAB = create_closure(ab);
    FuncBlock blockAB(closureAB);

    edgeC.from() = blockA;
    edgeC.to() = blockAB;

    edgeD.from() = blockAB;
    edgeD.to()   = blockB;

    cout << "last edgeD.id=" << edgeD.id << endl;
    cout << "last edgeC.id=" << edgeC.id << endl;

    start_session();
#endif
}
