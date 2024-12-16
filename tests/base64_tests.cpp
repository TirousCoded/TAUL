

#include <gtest/gtest.h>

#include <taul/base64.h>


// TODO: we likely could make the below more *robust* and *comprehensive*

TEST(Base64Tests, EncodeBase64) {
    // got some of these from https://en.wikipedia.org/wiki/Base64

    EXPECT_EQ(taul::encode_base64(""), "");

    EXPECT_EQ(taul::encode_base64("0123456789"), "MDEyMzQ1Njc4OQ==");
    EXPECT_EQ(taul::encode_base64("abcdefghijklmnopqrstuvwxyz"), "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXo=");
    EXPECT_EQ(taul::encode_base64("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), "QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVo=");

    EXPECT_EQ(taul::encode_base64("cos((a + b) * pi * 1.6)^0x14Ae == nil"), "Y29zKChhICsgYikgKiBwaSAqIDEuNileMHgxNEFlID09IG5pbA==");

    EXPECT_EQ(taul::encode_base64("Many hands make light work."), "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu");

    EXPECT_EQ(taul::encode_base64("light work."), "bGlnaHQgd29yay4=");
    EXPECT_EQ(taul::encode_base64("light work"), "bGlnaHQgd29yaw==");
    EXPECT_EQ(taul::encode_base64("light wor"), "bGlnaHQgd29y");
    EXPECT_EQ(taul::encode_base64("light wo"), "bGlnaHQgd28=");
    EXPECT_EQ(taul::encode_base64("light w"), "bGlnaHQgdw==");
    
    // this *should* encode as "FKMABGc7xv/hv5GqKA==", I think...
    std::vector<uint8_t> some_data{
        0x14, 0xa3, 0x00, 0x04, 0x67, 0x3b, 0xc6, 0xff, 0xe1, 0xbf, 0x91, 0xaa, 0x28
    };

    EXPECT_EQ(taul::encode_base64(some_data), "FKMABGc7xv/hv5GqKA==");
}

static auto conv(std::string_view x) {
    return std::make_optional(std::vector<uint8_t>(x.begin(), x.end()));
}
static auto conv(const std::vector<uint8_t>& x) {
    return std::make_optional(x);
}

TEST(Base64Tests, DecodeBase64) {
    // got some of these from https://en.wikipedia.org/wiki/Base64

    EXPECT_EQ(taul::decode_base64(""), conv(""));

    EXPECT_EQ(taul::decode_base64("MDEyMzQ1Njc4OQ=="), conv("0123456789"));
    EXPECT_EQ(taul::decode_base64("MDEyMzQ1Njc4OQ"), conv("0123456789")); // no padding
    EXPECT_EQ(taul::decode_base64("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXo="), conv("abcdefghijklmnopqrstuvwxyz"));
    EXPECT_EQ(taul::decode_base64("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXo"), conv("abcdefghijklmnopqrstuvwxyz")); // no padding
    EXPECT_EQ(taul::decode_base64("QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVo="), conv("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
    EXPECT_EQ(taul::decode_base64("QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVo"), conv("ABCDEFGHIJKLMNOPQRSTUVWXYZ")); // no padding

    EXPECT_EQ(taul::decode_base64("Y29zKChhICsgYikgKiBwaSAqIDEuNileMHgxNEFlID09IG5pbA=="), conv("cos((a + b) * pi * 1.6)^0x14Ae == nil"));
    EXPECT_EQ(taul::decode_base64("Y29zKChhICsgYikgKiBwaSAqIDEuNileMHgxNEFlID09IG5pbA"), conv("cos((a + b) * pi * 1.6)^0x14Ae == nil")); // no padding

    EXPECT_EQ(taul::decode_base64("TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"), conv("Many hands make light work."));

    EXPECT_EQ(taul::decode_base64("bGlnaHQgd29yay4="), conv("light work."));
    EXPECT_EQ(taul::decode_base64("bGlnaHQgd29yay4"), conv("light work.")); // no padding
    EXPECT_EQ(taul::decode_base64("bGlnaHQgd29yaw=="), conv("light work"));
    EXPECT_EQ(taul::decode_base64("bGlnaHQgd29yaw"), conv("light work")); // no padding
    EXPECT_EQ(taul::decode_base64("bGlnaHQgd29y"), conv("light wor"));
    EXPECT_EQ(taul::decode_base64("bGlnaHQgd28="), conv("light wo"));
    EXPECT_EQ(taul::decode_base64("bGlnaHQgd28"), conv("light wo")); // no padding
    EXPECT_EQ(taul::decode_base64("bGlnaHQgdw=="), conv("light w"));
    EXPECT_EQ(taul::decode_base64("bGlnaHQgdw"), conv("light w")); // no padding
    
    // this *should* encode as "FKMABGc7xv/hv5GqKA==", I think...
    std::vector<uint8_t> some_data{
        0x14, 0xa3, 0x00, 0x04, 0x67, 0x3b, 0xc6, 0xff, 0xe1, 0xbf, 0x91, 0xaa, 0x28
    };

    EXPECT_EQ(taul::decode_base64("FKMABGc7xv/hv5GqKA=="), conv(some_data));
    EXPECT_EQ(taul::decode_base64("FKMABGc7xv/hv5GqKA"), conv(some_data)); // no padding

    EXPECT_EQ(taul::decode_base64("@#$"), std::nullopt); // invalid Base64
    EXPECT_EQ(taul::decode_base64("*)("), std::nullopt); // invalid Base64
    EXPECT_EQ(taul::decode_base64("~`"), std::nullopt); // invalid Base64
    EXPECT_EQ(taul::decode_base64("\r\t\n"), std::nullopt); // invalid Base64
    EXPECT_EQ(taul::decode_base64("===="), std::nullopt); // invalid Base64
}

