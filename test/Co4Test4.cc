
#include "TestHelpers.h"

namespace {
const QString ALF("Alf");
const QString TYPE1("type1"), TYPE2("type2");

class Co4Test4 : public ClientFixture {
public:
    Co4Test4();
    CoClient_x c1, c2, c3;
    MessageReceiver_x mc1, mc2, mc3;
};

Co4Test4::Co4Test4()
    : ClientFixture("local:///" TEST_BUILDDIR "test4.sock")
{
    c1 = addClient(mc1, TYPE1, ALF);
    c2 = addClient(mc2, TYPE2, ALF);
    c3 = addClient(mc3, TYPE1, ALF);
    clearCounts();
}

} // namespace

TEST_F(Co4Test4, ThreeClients)
{
    miQMessage msg("from A1 to A2");
    c1->sendMessage(msg, clientId(c2->getClientId()));
    test_wait();

    EXPECT_EQ(0, mc1->count());
    EXPECT_EQ(1, mc2->count());
    EXPECT_EQ(0, mc3->count());
}
