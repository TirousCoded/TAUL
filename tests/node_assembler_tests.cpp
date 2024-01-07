

#include <gtest/gtest.h>

#include <taul/all.h>


// TODO: expand this test suite later

// TODO: I haven't tested node_assembler being reusable, so we'll need to write those later, lol


TEST(NodeAssemblerTests, BasicUsage) {

    auto lgr = taul::make_stderr_logger();

    taul::spec spec =
        taul::spec_writer()
        .lpr_decl("lpr")
        .ppr_decl("ppr")
        .lpr("lpr")
        .close()
        .ppr("ppr")
        .close()
        .done();

    taul::grammar gram = std::move(*taul::load(spec, lgr));

    taul::node_ctx ctx;


    auto c = ctx.create(gram.ppr("ppr"), "c", 2);
    c.attach(ctx.create(gram.lpr("lpr"), "c", 2));

    auto bcd = ctx.create(gram.ppr("ppr"), "bcd", 1);
    bcd.attach(ctx.create(gram.lpr("lpr"), "b", 1));
    bcd.attach(std::move(c));
    bcd.attach(ctx.create(gram.lpr("lpr"), "d", 3));

    auto tree1 = ctx.create(gram.ppr("ppr"), "abcde");
    tree1.attach(ctx.create(gram.lpr("lpr"), "a", 0));
    tree1.attach(std::move(bcd));
    tree1.attach(ctx.create(gram.lpr("lpr"), "e", 4));

    
    taul::node_assembler na(lgr);

    auto tree2 = na
        .begin(ctx.create(gram.ppr("ppr"), "abcde"))
        .attach(ctx.create(gram.lpr("lpr"), "a", 0))
        .enter(ctx.create(gram.ppr("ppr"), "bcd", 1))
        .attach(ctx.create(gram.lpr("lpr"), "b", 1))
        .enter(ctx.create(gram.ppr("ppr"), "c", 2))
        .attach(ctx.create(gram.lpr("lpr"), "c", 2))
        .exit()
        .attach(ctx.create(gram.lpr("lpr"), "d", 3))
        .exit()
        .attach(ctx.create(gram.lpr("lpr"), "e", 4))
        .end();


    EXPECT_TRUE(tree1.equiv(tree2));
}

