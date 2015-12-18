
#include "TestHelpers.h"

namespace {
const QString ALF("Alf"), BERT("Bert");
const QString TYPE1("type1"), TYPE2("type2"), TYPEBC("watcher");

class Co4Test2 : public ClientFixture {
public:
    Co4Test2();
    CoClient_x ca1, ca2, ca3, cb1;
    MessageReceiver_x mca1, mca2, mca3, mcb1;
};

Co4Test2::Co4Test2()
    : ClientFixture("local:///" TEST_BUILDDIR "test2.sock")
{
    ca1 = addClient(mca1, TYPE1, ALF);
    ca2 = addClient(mca2, TYPE2, ALF);
    ca3 = addClient(mca3, TYPE2, ALF);
    cb1 = addClient(mcb1, TYPE1, BERT);
    clearCounts();
}

} // namespace


TEST_F(Co4Test2, A1ToA)
{
    miQMessage msg("from A1 to A");
    ca1->sendMessage(msg);
    test_wait();

    EXPECT_EQ(0, mca1->count());
    EXPECT_EQ(1, mca2->count());
    EXPECT_EQ(1, mca3->count());
    EXPECT_EQ(0, mcb1->count());
}

TEST_F(Co4Test2, A1ToA2)
{
    miQMessage msg("from A1 to A2");
    ca1->sendMessage(msg, clientId(ca2->getClientId()));
    test_wait();

    EXPECT_EQ(0, mca1->count());
    EXPECT_EQ(1, mca2->count());
    EXPECT_EQ(0, mca3->count());
    EXPECT_EQ(0, mcb1->count());
}

TEST_F(Co4Test2, A1ToB1)
{
    miQMessage msg("from A1 to B1");
    ca1->sendMessage(msg, clientId(cb1->getClientId()));
    test_wait();

    EXPECT_EQ(0, mca1->count());
    EXPECT_EQ(0, mca2->count());
    EXPECT_EQ(0, mca3->count());
    EXPECT_EQ(0, mcb1->count()); // different user
}
