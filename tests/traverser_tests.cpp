

#include <gtest/gtest.h>

#include <taul/all.h>


class test_traverser final : public taul::traverser {
public:

    std::string events;
    std::string skip_if_str_eq;


    inline test_traverser() 
        : traverser() {}


    inline void on_begin() override final {
        events += "begin\n";
    }

    inline void on_end() override final {
        events += "end\n";
    }

    inline void on_enter(const taul::node& nd, bool& skip_children) override final {
        if (nd.str() == skip_if_str_eq) {
            skip_children = true;
        }
        events += std::format("enter {}\n", (std::string)nd.str());
    }

    inline void on_exit(const taul::node& nd) override final {
        events += std::format("exit {}\n", (std::string)nd.str());
    }
};


class TraverserTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr = nullptr;
    std::shared_ptr<test_traverser> trav = nullptr;

    taul::grammar gram;
    taul::node_ctx ctx;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        trav = std::make_shared<test_traverser>();

        auto spec =
            taul::spec_writer()
            .lpr_decl("lpr")
            .ppr_decl("ppr")
            .lpr("lpr")
            .close()
            .ppr("ppr")
            .close()
            .done();

        gram = std::move(*taul::load(spec, lgr));
    }
};


TEST_F(TraverserTests, BasicTraversal) {

    taul::node p1 = ctx.create(gram.ppr("ppr"), "p1");
    p1.attach(ctx.create(gram.lpr("lpr"), "l1"));
    p1.attach(ctx.create(gram.lpr("lpr"), "l2"));

    taul::node p2 = ctx.create(gram.ppr("ppr"), "p2");
    p2.attach(ctx.create(gram.lpr("lpr"), "l4"));
    p2.attach(ctx.create(gram.lpr("lpr"), "l5"));

    taul::node p0 = ctx.create(gram.ppr("ppr"), "p0");
    p0.attach(ctx.create(gram.lpr("lpr"), "l0"));
    p0.attach(std::move(p1));
    p0.attach(ctx.create(gram.lpr("lpr"), "l3"));
    p0.attach(std::move(p2));
    p0.attach(ctx.create(gram.lpr("lpr"), "l6"));


    std::string expected{};
    expected += "begin\n";
    expected += "enter p0\n";
    expected += "enter l0\n";
    expected += "exit l0\n";
    expected += "enter p1\n";
    expected += "enter l1\n";
    expected += "exit l1\n";
    expected += "enter l2\n";
    expected += "exit l2\n";
    expected += "exit p1\n";
    expected += "enter l3\n";
    expected += "exit l3\n";
    expected += "enter p2\n";
    expected += "enter l4\n";
    expected += "exit l4\n";
    expected += "enter l5\n";
    expected += "exit l5\n";
    expected += "exit p2\n";
    expected += "enter l6\n";
    expected += "exit l6\n";
    expected += "exit p0\n";
    expected += "end\n";


    taul::traverse(*trav, p0);

    TAUL_LOG(lgr, "expected:\n{}\nactual:\n{}\n", expected, trav->events);

    EXPECT_EQ(trav->events, expected);
}


TEST_F(TraverserTests, TraversalWithSkipChildren) {

    taul::node p1 = ctx.create(gram.ppr("ppr"), "p1");
    p1.attach(ctx.create(gram.lpr("lpr"), "l1"));
    p1.attach(ctx.create(gram.lpr("lpr"), "l2"));

    taul::node p2 = ctx.create(gram.ppr("ppr"), "p2");
    p2.attach(ctx.create(gram.lpr("lpr"), "l4"));
    p2.attach(ctx.create(gram.lpr("lpr"), "l5"));

    taul::node p0 = ctx.create(gram.ppr("ppr"), "p0");
    p0.attach(ctx.create(gram.lpr("lpr"), "l0"));
    p0.attach(std::move(p1));
    p0.attach(ctx.create(gram.lpr("lpr"), "l3"));
    p0.attach(std::move(p2));
    p0.attach(ctx.create(gram.lpr("lpr"), "l6"));


    std::string expected{};
    expected += "begin\n";
    expected += "enter p0\n";
    expected += "enter l0\n";
    expected += "exit l0\n";
    expected += "enter p1\n";
    //expected += "enter l1\n";
    //expected += "exit l1\n";
    //expected += "enter l2\n";
    //expected += "exit l2\n";
    expected += "exit p1\n";
    expected += "enter l3\n";
    expected += "exit l3\n";
    expected += "enter p2\n";
    expected += "enter l4\n";
    expected += "exit l4\n";
    expected += "enter l5\n";
    expected += "exit l5\n";
    expected += "exit p2\n";
    expected += "enter l6\n";
    expected += "exit l6\n";
    expected += "exit p0\n";
    expected += "end\n";


    trav->skip_if_str_eq = "p1";

    taul::traverse(*trav, p0);

    TAUL_LOG(lgr, "expected:\n{}\nactual:\n{}\n", expected, trav->events);

    EXPECT_EQ(trav->events, expected);
}

