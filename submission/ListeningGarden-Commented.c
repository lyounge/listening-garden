/* Grow a (Feeling) Garden | gcc -std=c11 ListeningGarden.c -o GardenClass.exe
   บันทึกซอร์สเป็น UTF-8 เพื่อแสดงภาษาไทยและภาพจุด  */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  /* ใช้เวลาเริ่มต้นลำดับเลขสุ่มเพียงครั้งเดียว */

#define TYPES 6
#define VISITS 8
#define STOCK 12
/* ผลึกเรียงเป็นคู่ 0–1 ใบ, 2–3 ราก, 4–5 เปลือก */
typedef struct {
    int a, b;              /* b เท่ากับ -1 เมื่อใช้ผลึกตรง ๆ */
} Move;

typedef struct {
    int bag[TYPES];        /* ผลึกดิบที่เหลือ */
    int value[3];          /* หน่วยที่ใส่ให้ใบ ราก เปลือก */
    int count;
    Move moves[STOCK];     /* จำสิ่งที่ทำไว้สำหรับปุ่มย้อน */
} Plan;

typedef struct {
    int weather, kind;
    int guest[VISITS], gift[VISITS], stock[TYPES];
    int changed_gift;
    Plan answer;           /* แผนที่ตรวจแล้ว เก็บไว้เฉลยตอนจบ */
} Day;

typedef struct {
    double growth, balance, weatherScore;
    int missing[3], health;
    int perfect;
} Result;

typedef struct {
    int type;
    const char *name, *story, *clarify, *reply;
    const char *question, *otherQuestion, *otherReply, *nextMorning, *listenAction;
} Story;

Move make_move(int a, int b) {
    Move move={a,b};
    return move;
}

/* เรื่องของลูกค้า */
const Story stories[] = {
    {0, "นิด · คนขายขนม",
     "เมื่อเช้ามีคนเรียก “ป้านิด” หันไปนึกว่าใคร\n  เจ้าเด็กที่เคยซื้อขนมก่อนไปโรงเรียนนั่นเอง เดี๋ยวนี้อุ้มลูกมาแล้ว\n  ฉันนี่จำหน้าแทบไม่ได้",
     "ขออวดหน่อยเถอะ ยืนยิ้มคนเดียวมาตั้งแต่เช้าแล้ว",
     "เขายังถามหาชิ้นริมอยู่เลยนะ ฉันก็ว่าเอ๊ะ ยังจำได้อีกเหรอ\n  ลูกเขาเห็นพ่อกินก็จะเอาบ้าง เลยได้แถมไปสองชิ้น",
     "เขายังซื้อขนมแบบเดิมอยู่ไหม", "เมื่อก่อนเขามาซื้อบ่อยเหรอ",
     "แทบทุกเช้าเลย ชอบมาถึงตอนฉันยังตัดไม่เสร็จ ยืนมองมีดอยู่นั่นแหละ\n  บอกให้ไปนั่งก็ไม่ไป",
     "วันนี้เขาไม่ได้แวะนะ แต่ฉันยังตัดชิ้นริมเผื่อไว้อยู่เลย",
     "คุณนั่งฟัง นิดหัวเราะแล้วเล่าต่อ"},
    {1, "เมฆ · ช่างไม้",
     "โต๊ะขายออกแล้วครับ ตัวที่ผมทำอยู่นานนั่นแหละ\n  ตอนเขาจ่ายเงินผมนับอยู่สองรอบ ไม่ใช่อะไรนะ \n  ไม่คิดว่าจะมีคนซื้อจริง ๆ",
     "ขอเล่าเรื่องลูกค้าหน่อยครับ ยังไม่ได้เล่าให้ใครฟังเลย",
     "เขาถามว่ารับทำอีกตัวไหม ผมรีบตอบตกลงเลยครับ\n  ค่อยกลับมานั่งคิดว่าจะทำทันหรือเปล่า สงสัยต้องโทรไปคุยวันส่งใหม่",
     "ตอนซื้อ ลูกค้าเขาว่ายังไงบ้าง", "ทำส่วนไหนนานที่สุด",
     "ขาโต๊ะครับ ตอนแรกวางแล้วโยก ผมไสไปไสมาจนเตี้ยกว่าที่วาดไว้\n  ต้องแก้ใหม่หมด ตอนนี้ลองโยกยังไงก็ไม่ขยับแล้ว",
     "โทรคุยวันส่งแล้วครับ เขารอได้ ค่อยโล่งหน่อย",
     "เมฆเลื่อนแก้วหลบ แล้วกางมือให้ดูขนาดโต๊ะ"},
    {0, "อุ่น · คนส่งจดหมาย",
     "ขอน้ำหน่อยครับ เดินถามทางมาครึ่งซอย คอแห้งหมดแล้ว\n  วันนี้หาบ้านผู้รับเจอสักที ที่อยู่บนซองใช้เลขบ้านเก่า\n  ผมวนอยู่หลายรอบ",
     "ขอนั่งแป๊บหนึ่งครับ เดี๋ยวเล่าให้ฟัง กว่าจะเจอนี่ถามไปครึ่งซอย",
     "ได้ป้าร้านชำช่วยบอกทางครับ ผมเคยเดินผ่านบ้านนั้นด้วยนะ\n  แต่มัวแต่หาเลขบนซอง เขาออกมารับผมเลยถามชื่อซ้ำตั้งสองที\n  กลัวผิดบ้านอีก",
     "สุดท้ายหาเจอได้ยังไง", "แล้วเจ้าของจดหมายว่าไงบ้าง",
     "เขาบอกว่ารออยู่พอดีครับ หายเข้าไปหยิบแว่นแล้วกลับมาอ่านตรงประตูเลย\n  ผมก็อยากรู้ว่าในนั้นเขียนอะไร แต่ต้องไปส่งบ้านอื่นต่อแล้ว",
     "วันนี้ผ่านร้านชำ เลยแวะขอบคุณป้าแกหน่อย ไม่งั้นคงหาไม่เจอ",
     "คุณวางน้ำให้อุ่น เขาดื่มไปหลายอึกก่อนเล่าต่อ"},
    {1, "ฝน · นักว่ายน้ำ",
     "ดูแขนสิ ยกแทบไม่ขึ้นเลย เมื่อวานว่ายถึงอีกฝั่งแล้วนะ\n  ไม่ได้เกาะเชือกด้วย ตอนกลับบ้านอยากเล่าให้แม่ฟัง แต่แม่หลับไปแล้ว",
     "ขอเล่าอีกทีนะ เล่าแล้วก็ยังดีใจอยู่",
     "ครูบอกให้พักก่อน แต่ฉันเกาะขอบแล้วหันกลับไปมองฝั่งที่เพิ่งว่ายมา\n  มันก็ไม่ได้ไกลขนาดนั้นนี่นา ตอนอยู่กลางสระนึกว่าจะไม่ถึงแล้ว",
     "พอถึงอีกฝั่ง ทำอะไรก่อนเลย", "ตอนอยู่กลางสระกลัวไหม",
     "กลัวสิ เผลอกลืนน้ำไปทีหนึ่งด้วย เกือบจะคว้าเชือกแล้ว\n  แต่ได้ยินครูบอกว่าอีกนิดเดียว ก็เลยลองต่อ",
     "วันนี้เล่าให้แม่ฟังแล้ว แม่ถามว่ามีใครถ่ายคลิปไว้ไหม เสียดายเหมือนกัน",
     "ฝนวางกระเป๋าลงแล้วทำท่าเกาะขอบสระให้ดู"},
    {2, "ยายมะลิ · คนจัดดอกไม้",
     "ฝากดอกไม้ไว้หน่อยได้ไหมลูก ยายมัดมาเกินอีกแล้ว\n  เคยทำไว้ให้ตาแกถือกลับบ้าน พอตาไม่อยู่ มือก็ยังหยิบมาเท่าเดิมทุกที",
     "ขอนั่งด้วยสักพักนะ ยังไม่อยากกลับบ้านตอนนี้",
     "ฝากกำนี้ไว้ด้วยนะลูก ก้านเริ่มเหี่ยวแล้ว\n  อีกกำยายเอากลับเอง",
     "เอาดอกไม้ใส่น้ำก่อนไหมยาย", "เมื่อก่อนตามาช่วยจัดด้วยไหม",
     "ช่วยแกะยางน่ะสิ ให้จัดทีไรชอบยัดแน่นจนดอกช้ำ ยายบ่นแกประจำ\n  พอไม่มีคนทำให้บ่นก็เงียบไปอีก",
     "ยายเอายางมาคืน เมื่อวานติดกลับไปด้วย ดอกไม้ยังอยู่ดีไหมลูก",
     "คุณเลื่อนเก้าอี้ให้ยาย แล้วรับดอกไม้ไปใส่แจกัน"},
    {3, "ป่าน · ครูฝึกสอน",
     "ขอนั่งซ้อมพูดตรงนี้ได้ไหมคะ พรุ่งนี้สอนเองครั้งแรก\n  เมื่อกี้ลองพูดหน้ากระจกแล้วลืมแม้แต่ชื่อตัวเอง\n  ขนาดยังไม่มีเด็กนั่งฟังนะ",
     "ได้ค่ะ งั้นขอลองแค่ตอนทักทายก่อนนะคะ",
     "งั้นถามชื่อทีละคนก่อนเนอะ ฉันจะได้มีเวลาหายใจบ้าง\n  แต่ถ้าเด็กถามกลับว่าครูตื่นเต้นเหรอ คงต้องบอกว่ามากเลยค่ะ",
     "พรุ่งนี้คิดว่าจะเริ่มคุยกับเด็ก ๆ ยังไง", "กลัวอะไรที่สุดตอนเข้าห้อง",
     "กลัวถามแล้วไม่มีใครตอบค่ะ ต้องยืนยิ้มอยู่คนเดียว\n  เคยเห็นครูพี่เลี้ยงรอได้ตั้งนาน ฉันคงรีบตอบเองหมด",
     "เด็กถามจริง ๆ ว่าครูตื่นเต้นเหรอ ฉันบอกว่าใช่ เขาก็หัวเราะกัน\n  แล้วก็มีคนยกมือช่วยตอบ",
     "คุณนั่งหันหน้าเข้าหาป่าน ให้เธอลองทักทายเหมือนอยู่หน้าห้อง"},
    {2, "กล้า · คนดูแลสนาม",
     "ต้นหูกวางข้างสนามล้มแล้วครับ เมื่อเช้าช่วยเขาเก็บกิ่งอยู่\n  ตรงนั้นโล่งจนผมเดินผ่านแล้วไม่ชิน\n  ตอนเด็กเล่นอยู่ใต้ต้นนั้นแทบทุกวัน",
     "ขอเล่าเรื่องต้นนั้นหน่อยครับ กลับไปที่สนามก็คงต้องเก็บต่อ",
     "เจอลูกแก้วตอนโกยดินครับ ไม่รู้ของผมหรือเปล่า\n  สมัยก่อนซ่อนกันไว้ตามราก ตอนนี้ล้างแล้วใส่กระเป๋าไว้\n  เดี๋ยวเอาให้น้องดู",
     "ตอนเก็บกิ่งเจอของเก่าบ้างไหม", "เมื่อก่อนเล่นอะไรตรงนั้น",
     "เล่นลูกแก้วกับน้องครับ แพ้ทีไรก็หาว่ามันโกง\n  สุดท้ายแม่ต้องมาตามกลับบ้านทั้งคู่\n  จำได้ว่ากลับไปโดนดุเรื่องเสื้อเปื้อนอีก",
     "น้องบอกจำลูกแก้วไม่ได้แล้ว แต่ขอเก็บไว้ ผมก็ให้มันไป",
     "คุณรอให้กล้าวางถุงมือ เขาล้วงของเล็ก ๆ ออกจากกระเป๋า"},
    {3, "ไหม · นักเดินทาง",
     "รถยังไม่มาหรอกใช่ไหมคะ ฉันดูเวลาไปสามรอบแล้ว\n  นี่เดินทางคนเดียวครั้งแรก แม่โทรมาถามก็บอกว่าสบายมาก\n  แต่ตอนนี้อยากให้แม่มาส่งจัง",
     "ขอนั่งฝั่งที่เห็นป้ายนะคะ กลัวเผลอแล้วมองไม่ทัน",
     "ขอบคุณค่ะ งั้นนั่งตรงนี้นะ ตั๋วก็อยู่ กระเป๋าก็ครบ เหลือแต่ขึ้นให้ถูกคัน\n  ถ้าเห็นรถมาก่อนช่วยสะกิดด้วยนะ",
     "นั่งตรงนี้ไหม มองเห็นป้ายพอดี", "ไปถึงแล้วมีใครมารับไหม",
     "มีเพื่อนมารับค่ะ ส่งรูปทางออกมาให้แล้ว ฉันบันทึกไว้หมดเลย\n  แต่ก็ยังกลัวเดินออกผิดฝั่งอยู่ดี",
     "ถึงแล้วนะคะ ออกผิดฝั่งจริงด้วย แต่โทรหาเพื่อนแล้วเจอกันแล้ว",
     "คุณขยับเก้าอี้ให้ไหมนั่งตรงที่มองเห็นป้ายรถ"},
    {4, "เต้ · ช่างป้าย",
     "ผมวาดป้ายจนดึก พอไปถึงร้านเช้านี้ เขาทาสีทับไปแล้วครับ\n  บอกว่าเจ้าของอยากได้อีกแบบ แล้วทำไมไม่โทรมาบอกก่อน\n  ผมถือพู่กันยืนอยู่ตั้งนาน",
     "ขอบ่นตรงนี้ก่อนนะครับ ถ้าเดินไปหาเขาตอนนี้คงเสียงดังแน่",
     "ผมอยากถามแค่ว่ารู้ตั้งแต่เมื่อไหร่ ถ้ารู้เมื่อวาน\n  ผมจะได้ไม่ต้องนั่งทำจนดึก แก้แบบน่ะผมแก้ให้ได้ แค่โทรมาบอกกันหน่อย",
     "ถ้าได้คุย อยากถามเขาเรื่องไหนก่อน", "เขาทาทับหมดเลยเหรอ",
     "เหลือขอบนิดเดียวครับ ตรงตัวหนังสือหายหมด ผมถ่ายรูปไว้เมื่อคืน\n  ยังดีที่ถ่าย ไม่งั้นไม่มีอะไรให้ดูแล้ว",
     "ยังไม่ได้คุยยาวครับ วันนี้เขารีบ ผมเลยนัดเวลาไว้ก่อน\n  ไม่อยากคุยกันตอนเดินผ่าน",
     "เต้เปิดรูปป้ายเมื่อคืนให้ดู แล้วชี้ตรงขอบที่เหลืออยู่"},
    {5, "ดาว · นักร้อง",
     "เมื่อคืนไมค์ดับค่ะ ตรงท่อนที่ซ้อมมาเยอะที่สุดเลย\n  ตอนลงเวทีก็ยิ้มให้คนอื่นนะ แต่กลับบ้านแล้วเปิดคลิปดูไม่ได้สักที",
     "ขอเล่าให้จบก่อนได้ไหม ยังไม่อยากได้ยินว่าคราวหน้าค่อยเอาใหม่",
     "ร้องต่อค่ะ คนแถวหน้าร้องท่อนนั้นอยู่ ฉันเลยร้องตามเขาไปจนจบ\n  แต่ตอนนั้นในหัวคิดแต่ว่าทำไมต้องดับตอนนี้ด้วย",
     "แล้วตอนนั้นร้องต่อจนจบไหม", "เสียดายท่อนไหนที่สุด",
     "ท่อนที่แม่ชอบค่ะ ชวนแม่มาฟังด้วย ตอนกลับแม่บอกว่าสนุก\n  แต่ฉันยังหงุดหงิดจนตอบแม่ไปนิดเดียว รู้สึกผิดอยู่เหมือนกัน",
     "ยังไม่ได้ดูคลิปนะ แต่โทรคุยกับแม่แล้ว แม่บอกได้ยินฉันร้องอยู่\n  แค่นั่งไกลไปหน่อย",
     "คุณวางแก้วลงแล้วฟังต่อ ดาวหยุดนิดหนึ่งก่อนเล่าถึงตอนอยู่บนเวที"},
    {4, "อิฐ · คนล้างจาน",
     "ผมเพิ่งเข้าครัว เขาก็บอกว่าทำจานแตกอีกแล้ว ผ้ายังไม่ได้ผูกเลยครับ\n  จะให้ผมทำแตกตอนไหน พอจะอธิบายก็บอกให้รีบเก็บก่อน",
     "ฟังผมก่อนนะครับ ผมเล่าเรื่องนี้ให้ใครฟังก็มีแต่คนบอกว่าช่างมัน",
     "มีคนอยู่หน้าอ่างสองคนครับ ผมยังเดินไปไม่ถึงอ่างเลย\n  อยากบอกแค่ว่าเห็นหรือเปล่าว่าผมทำ ถ้าไม่เห็นก็อย่าเพิ่งว่ากัน",
     "ตอนเดินเข้าไป มีใครอยู่บ้าง", "แล้วได้คุยกันอีกรอบไหม",
     "ยังครับ ลูกค้าเข้าพอดี ทุกคนก็แยกไปทำงาน ผมเลยยังคาใจอยู่\n  เขาอาจลืมไปแล้วด้วยซ้ำ",
     "ได้บอกแล้วครับว่าเมื่อวานผมไม่ได้ทำ เขาตอบแค่ว่าอ๋อ\n  ผมก็ยังหงุดหงิดอยู่นะ",
     "คุณให้อิฐเล่าตั้งแต่เดินเข้าครัว โดยยังไม่ขัด"},
    {5, "หลิว · คนเพาะกล้า",
     "ช่วยดูถาดนี้หน่อยสิ รอมาตั้งหลายวัน ไม่ขึ้นสักต้น\n  คนอื่นหว่านทีหลังใบออกกันแล้ว ฉันเดินไปดูทุกเช้าจนไม่อยากดูแล้ว",
     "ขอวางไว้ตรงนี้ก่อนนะ หิ้วมาแล้วก็ยังไม่รู้จะทำยังไงต่อ",
     "ขอพักก่อนดีกว่า วันนี้เห็นถุงเมล็ดก็เซ็งแล้ว\n  เดี๋ยวเอาถาดนี้กลับไปดูก่อนว่าข้างล่างแฉะไหม\n  เมื่อวานฉันก็เพิ่งรดน้ำเพิ่มอีก",
     "จะลองหว่านใหม่เลยไหม หรือพักก่อน", "รดน้ำไปเยอะไหม",
     "อาจจะเยอะนะ เห็นหน้าดินแห้งก็เติม ไม่ได้ดูข้างล่างเลย\n  พูดแล้วอยากกลับไปยกถาดดู แต่ถ้าเน่าหมดก็คงเสียดายอีก",
     "ยกถาดดูแล้ว น้ำขังจริง ๆ ยังไม่ได้หว่านใหม่ ขอเอาถาดไปผึ่งก่อน",
     "คุณช่วยหลิววางถาดบนพื้น เธอเช็ดมือแล้วนั่งลง"}
};

/* ผลึก คะแนน และการสุ่มวัน */
const char crystal[][64] = {"สุข","ภูมิใจ","เศร้า","กลัว","โกรธ","ผิดหวัง"};
const char part[][64] = {"ใบและดอก","ราก","เปลือก"};
const char weather[][64] = {"พายุ","แล้ง","แมลง"};
const char forecast[][512] = {
 "คืนนี้มีพายุ ลมทำให้ต้นล้มและกิ่งเสียหาย\n  ราก: อย่างน้อย 30 และไม่น้อยกว่าใบ\n  เปลือก: อย่างน้อย 20 และไม่น้อยกว่าครึ่งหนึ่งของใบ",
 "คืนนี้แล้ง รากต้องส่งน้ำให้ใบ\n  ราก: อย่างน้อย 30 และไม่น้อยกว่าใบ",
 "คืนนี้มีแมลง เปลือกช่วยป้องกันต้นไม้\n  เปลือก: อย่างน้อย 30 และไม่น้อยกว่าใบ"};
const char *mix_names[6][6]={
  {0,0,"อบอุ่นใจ","กล้าหาญ","มุ่งมั่น","มีหวัง"},
  {0,0,"ยอมรับตัวเอง","เชื่อมั่นในตัวเอง","เคารพตัวเอง","หนักแน่น"},
  {0,0,0,0,"ฮึดสู้","เห็นอกเห็นใจ"},
  {0,0,0,0,"เด็ดเดี่ยว","ทำใจยอมรับ"}};

const char *recipe(int a,int b) {
 if (a <= b) return mix_names[a][b];
 return mix_names[b][a];
}
int target(Move m) {
    int first = m.a / 2;
    if (m.b == -1) return first;
    int second = m.b / 2;
    /* เลือกกลุ่มที่ต่างจากวัตถุดิบทั้งสองชิ้น */
    for (int group = 0; group < 3; group++) {
        if (group != first && group != second) return group;
    }
    return -1;
}
/* แก้แผนจริงผ่าน pointer และจำวัตถุดิบเพื่อย้อน */
int apply(Plan *p,Move m) {
 if(m.a<0 || m.a>=TYPES || m.b < -1 || m.b>=TYPES || !p->bag[m.a] || p->count>=STOCK) return 0;
 if(m.b>=0 && (m.a/2==m.b/2 || !p->bag[m.b])) return 0;
 p->bag[m.a]--;
 if(m.b>=0) p->bag[m.b]--;
 p->value[target(m)]+=10;
 p->moves[p->count++]=m;
 return 1;
}
void undo(Plan *p) {
 if(!p->count) return;
 Move m=p->moves[--p->count];
 p->bag[m.a]++;
 if(m.b>=0) p->bag[m.b]++;
 p->value[target(m)]-=10;
}
int larger(int a, int b) {
    if (a>b) return a;
    return b;
}

int smaller(int a, int b) {
    if (a<b) return a;
    return b;
}

/* คำนวณผลจากปุ๋ยสามส่วนและอากาศของรอบนี้ */
const int challenge[3][3]={{20,40,30},{30,40,20},{30,20,40}};

int challenge_met(const int value[3], int weather_type) {
    for (int i=0;i<3;i++)
        if (value[i]<challenge[weather_type][i]) return 0;
    return 1;
}

Result evaluate(const int v[3],int w) {
 Result r={0};
 int low=v[0];
 double progress=1.0;
 for(int i=0;i<3;i++) {
  low=smaller(low,v[i]);
  r.growth+=smaller(v[i],challenge[w][i])*10.0/challenge[w][i];
  double ratio=(double)v[i]/challenge[w][i];
  if (ratio<progress) progress=ratio;
 }
 int rootGoal = 0, barkGoal = 0;
 if (w == 0 || w == 1) rootGoal = larger(30, v[0]);
 if (w == 0) barkGoal = larger(20, v[0] / 2);
 if (w == 2) barkGoal = larger(30, v[0]);
 r.missing[1] = larger(0, rootGoal - v[1]);
 r.missing[2] = larger(0, barkGoal - v[2]);
 r.health=larger(0,100-2*(r.missing[1]+r.missing[2]));
 if (low<10) r.health=0; /* ทุกส่วนต้องได้รับปุ๋ยก่อน จึงจะอยู่รอด */
 r.weatherScore=.3*r.health;
 r.perfect=challenge_met(v,w) && r.health==100;
 r.balance=40*progress;
 return r;
}
/* หาผลึกที่ยังเหลือในกลุ่ม คืน -1 ถ้าใช้หมดแล้ว */
int remaining(const Plan *p,int group) {
 int first=group*2;
 if(p->bag[first]>0) return first;
 if(p->bag[first+1]>0) return first+1;
 return -1;
}
int find_answer(Day *d, int scarce, int mixes) {
 Plan p={0};
 for (int i=0; i<TYPES; i++) p.bag[i]=d->stock[i];
 /* ผสมให้ส่วนที่ขาดก่อน แล้วเติมผลึกตรงกลุ่มให้ครบด้านละ 30 */
 for (int i=0; i<mixes; i++) {
    int a=remaining(&p,(scarce+1)%3), b=remaining(&p,(scarce+2)%3);
    if (a<0 || b<0 || !apply(&p,make_move(a,b))) return 0;
 }
 for (int g=0; g<3; g++) {
    while (p.value[g]<challenge[d->weather][g]) {
        int a=remaining(&p,g);
        if (a<0 || !apply(&p,make_move(a,-1))) return 0;
    }
 }
 d->answer=p;
 return evaluate(p.value,d->weather).perfect;
}

/* ตัวอย่าง draw(3) ให้ผลเป็น 0, 1 หรือ 2 */
int draw(int count) {
    return rand() % count;
}

void shuffle(int ids[], int size) {
    for (int i=size-1; i>0; i--) {
        int j=draw(i+1), temp=ids[i];
        ids[i]=ids[j];
        ids[j]=temp;
    }
}

void new_day(Day *d) {
    for (;;) {
        Day empty={0};
        *d=empty;
        int roll=draw(10);
        int mixes=0;
        if (roll>=8) { d->kind=2; mixes=3; }
        else if (roll>=4) { d->kind=1; mixes=1; }
        d->weather=draw(3);

        /* เริ่มที่ด้านละสามชิ้น แล้วแทนชิ้นที่ต้องผสมด้วยวัตถุดิบสองกลุ่ม */
        int scarce=draw(3), left[3];
        for (int g=0;g<3;g++) left[g]=challenge[d->weather][g]/10;
        if (left[scarce]<mixes) continue;
        left[scarce]-=mixes;
        left[(scarce+1)%3]+=mixes;
        left[(scarce+2)%3]+=mixes;
        for (int i=0; i<STOCK-9-mixes; i++) {
            int group;
            if (mixes>0) group=(scarce+1+draw(2))%3;
            else group=draw(3);
            left[group]++;
        }

        /* สับบัตรชื่อ 12 ใบ แล้วลองหยิบ 8 ใบแรกจนแจกผลึกได้ครบทุกกลุ่ม */
        int ids[12], people[3], valid;
        for (int i=0; i<12; i++) ids[i]=i;
        do {
            shuffle(ids,12);
            for (int g=0; g<3; g++) people[g]=0;
            for (int i=0; i<VISITS; i++) people[stories[ids[i]].type/2]++;
            valid=1;
            for (int g=0; g<3; g++) {
                if (people[g]>left[g] || (left[g]>0 && people[g]==0)) valid=0;
            }
        } while (!valid);
        for (int i=0; i<VISITS; i++) {
            d->guest[i]=ids[i];
            d->gift[i]=1;
            left[stories[ids[i]].type/2]--;
        }
        for (int g=0; g<3; g++) {
            while (left[g]>0) {
                int i=draw(VISITS);
                if (stories[ids[i]].type/2==g) {
                    d->gift[i]++;
                    left[g]--;
                }
            }
        }
        for (int i=0; i<VISITS; i++) d->stock[stories[ids[i]].type]+=d->gift[i];
        if (find_answer(d,scarce,mixes)) return;
    }
}

/* ภาพต้นไม้ */
/* รูปหนึ่งรูปมี 8 บรรทัด เว้นแถวบนให้โคนต้นอยู่ที่เดิม */
const char pictures[6][8][120]={
    { /* seed */
        "",
        "",
        "",
        "",
        "",
        "",
        "            ⢀⡤⢤⡀",
        "            ⠻⠤⠞⠁"
    },
    { /* sprout */
        "",
        "",
        "",
        "",
        "",
        "              ⣠⢖⡶",
        "           ⠘⢿⣶⠋⠉",
        "             ⢸"
    },
    { /* sapling */
        "",
        "",
        "         ⣀⣀",
        "         ⢳⡈⠙⢦  ⢀⣠⠤⢤",
        "          ⠙⠲⠬⣇⢠⠋ ⢀⠎",
        "          ⢠⡤⠤⣜⡯⠤⠖⠋",
        "           ⠳⠤⠼⡇",
        "             ⢠⠇"
    },
    { /* branches */
        "",
        "         ⢠",
        "          ⢧⡀ ⡄     ⣠",
        "      ⢀    ⠙⠦⡇   ⣀⠜⠁",
        "      ⠈⠳⡀    ⣇⣠⠤⠚⠁",
        "        ⠈⠳⢤⣀ ⡗",
        "            ⠉⡇",
        "             ⡇"
    },
    { /* young */
        "",
        "           ⡼⠳⡄      ⣀⣀⡀",
        "           ⠹⣄⡽⢀⣤⡀ ⢠⠎⢁⡰⠃",
        "    ⠸⡉⠙⢦⢠⣄⡀ ⠈⢹⢸⡀⢳⢀⡞⠉⠉",
        "     ⠙⠦⠤⢿⡀⠙⡆ ⢸ ⣳⠞⠋",
        "         ⠙⠲⢧⣀⣸⠋⠁",
        "             ⢹",
        "             ⡞"
    },
    { /* canopy */
        "       ⢀⣀⣀⡀⣠⠖⠒⠒⢦⣠⠤⠤⣄",
        "    ⣀⣀⣰⠋  ⠉   ⠰⠋⠉⠁ ⠈⠧⠤⠤⣄",
        "   ⢸⠁     ⠞⠉⠉          ⠈⡆",
        " ⢀⡴⠋  ⣀⣀⡀           ⠞⠉⠉ ⠙⠲⡀",
        " ⠘⢦⡀ ⠘⠁     ⠠⡀  ⢠⠖⠒⠂    ⢀⡴⠁",
        "   ⣇⣀⣀⡀  ⠐⣄  ⢹⡀  ⡴⠊    ⢀⡼",
        "    ⠈ ⠙⠢⠴⠚⠛⠷⡤⢤⣧⣶⠟⠦⢤⠴⠊⠉⠉⠉",
        "            ⢱⠘⢰⠃"
    }
};

void draw_frame(const Plan *p, int frame, const Result *morning) {
    int image=0;                      /* เมล็ด */
    if (frame==1) image=1;            /* แตกหน่อ */
    if (frame==2) image=2;            /* ต้นกล้า */
    if (frame>=3) image=4;            /* มีใบ */
    if (frame>=4 && (p->value[0]>=30 || (morning && morning->perfect))) image=5;
    if (frame>=2 && p->value[0]==0) image=3;
    if (morning && morning->health==0 && p->count>0) image=3;
    int leaning=morning && morning->missing[1]>0 && p->value[0]>p->value[1];
    for (int row=0; row<8; row++) {
        /* กิ่งเอนออกจากโคนมากขึ้น ยิ่งอยู่ใกล้ยอด */
        if (leaning) printf("%*s",8-row,"");
        if (morning && morning->perfect && row==2)
            printf("   ⢸⠁  ✿  ⠞⠉⠉    ✿     ⠈⡆\n");
        else if (morning && morning->perfect && row==4)
            printf(" ⠘⢦⡀ ⠘⠁  ✿  ⠠⡀  ⢠⠖⠒⠂ ✿  ⢀⡴⠁\n");
        else printf("%s\n", pictures[image][row]);
    }

    if (leaning) printf("             ⢀⠎\n");
    else if (frame && morning && morning->missing[2]>0) printf("             ⡏⠄\n");
    else if (frame >= 3 && p->value[2] >= 30) printf("            ⢸⡇\n");
    else if (frame >= 2) printf("             ⡇\n");
    else printf("             ⠂\n");
    printf("  ⠒⠂⠐⠒⠒⠂⠐⠒⠂⠐⠒⠂⠸⠒⠂⠐⠒⠒⠂⠐⠒⠂⠐⠒\n");
    if (frame >= 3 && p->value[1] >= 30) {
        printf("        ⢀⡠⠔⠊⢹⠑⠢⢄⡀\n");
        printf("      ⠠⠔⠁    ⠘⠄   ⠉⠒⠤\n");
    } else {
        if (frame>0) printf("             ⠣\n");
        else printf("\n");
        printf("\n");
    }
}

void show_tree(const Plan *p, const Result *morning) {
    if (p->count>0) draw_frame(p,4,morning);
    else draw_frame(p,0,morning);
}

/* ทุกเมนูใช้สถานะนี้ร่วมกัน เมื่ออ่านต่อไม่ได้ main จะจบเกม */
int inputEnded=0;

/* อ่านจนจบบรรทัด ไม่ทิ้งตัวอักษรไปปนกับเมนูถัดไป */
void wait_enter(void) {
    int c;
    fflush(stdout);
    do { c=getchar(); } while (c!='\n' && c!=EOF);
    if (c==EOF) inputEnded=1;
}

void grow_tree(const Plan *p) {
    printf("\nสวนหลังร้าน\n");
    if (!p->count) {
        draw_frame(p,0,NULL);
        return;
    }
    int shown=0;
    for (int frame=0; frame<=4; frame++) {
        /* ข้ามภาพที่เหมือนช่วงก่อน ทั้งยอด ลำต้น และราก */
        if (frame==3 && p->value[0]==0 &&
            p->value[1]<30 && p->value[2]<30) continue;
        if (frame==4 && p->value[0]<30) continue;
        if (shown) {
            printf("กด Enter เพื่อดูต้นไม้โตต่อ\n");
            wait_enter();
            if (inputEnded) return;
        }
        draw_frame(p,frame,NULL);
        shown=1;
    }
}

/* เมนูและลำดับการเล่น */
/* อ่านทีละบรรทัด ถ้าพิมพ์ผิด ให้เลือกใหม่โดยไม่เสียผลึก */
/* ใช้หัวข้อรูปแบบเดียวกันทุกหน้าหลัก */
void heading(const char *title) {
    printf("\n  ----------------------------------------------------\n");
    printf("  %s\n", title);
    printf("  ----------------------------------------------------\n");
}

int choose(int low, int high) {
    char line[100], extra;
    int answer;
    for (;;) {
        printf("\n  เลือก > ");
        fflush(stdout);
        if (!fgets(line, sizeof line, stdin)) {
            inputEnded=1;
            return 0;
        }
        int i=0;
        while (line[i] && line[i]!='\n') i++;
        if (line[i]!='\n' && !feof(stdin)) {
            wait_enter();
            printf("ข้อความยาวเกินไป กรุณาพิมพ์เฉพาะเลขเมนู\n");
            if (inputEnded) return 0;
            continue;
        }
        /* เมนูมีไม่เกินสองหลัก ป้องกันเลขใหญ่เกินชนิด int */
        if (sscanf(line, "%2d %c", &answer, &extra) == 1 &&
            answer >= low && answer <= high) return answer;
        printf("เลือกเลข %d ถึง %d แล้วกด Enter\n", low, high);
    }
}

void care_notes(void) {

    printf("  ผลึกแต่ละกลุ่มเลี้ยงส่วนไหน?\n\n");
    printf("    ใบและดอก  : สุข / ภูมิใจ\n");
    printf("    ราก       : เศร้า / กลัว\n");
    printf("    เปลือก    : โกรธ / ผิดหวัง\n\n");
    printf("  ใช้ตรง      ผลึก 1 ชิ้น เพิ่ม 10 หน่วย\n");
    printf("  ผสมปุ๋ย     ใช้ผลึกต่างกลุ่ม 2 ชิ้น\n");
    printf("              เพิ่มให้กลุ่มที่เหลือ 10 หน่วย\n\n");
    printf("  เป้าหมายของการปลูก\n");
    printf("    ใบ ราก และเปลือกต้องได้อย่างน้อยส่วนละ 10\n");
    printf("    จากนั้นต้องเหลือสุขภาพหลังผ่านอากาศมากกว่า 0\n");
    printf("    ถ้าดูแลครบและสมดุล ต้นไม้จะออกดอก ได้ผล Perfect\n");
    printf("    ดอกจึงเป็นเป้าหมายพิเศษ ไม่ออกดอกก็ยังรอดได้\n\n");
    printf("  วิธีทำให้ต้นไม้ออกดอก\n");
    printf("    เติมใบ ราก และเปลือกให้ถึงเป้าออกดอกของคืนนี้\n");
    printf("    ดูตัวเลขของแต่ละส่วนได้ที่โต๊ะปลูก\n");
    printf("    และต้องผ่านคืนนี้โดยไม่เสียสุขภาพ\n");

}

void guide(const Day *day) {
    printf("\nสมุดที่ป้าอิงฝากไว้\n");
    care_notes();
    printf("%s\n", forecast[day->weather]);
}

int solve_trade(Day *trial) {
    int need[3];
    int mixes=0;
    for (int g=0;g<3;g++) {
        need[g]=larger(0,challenge[trial->weather][g]/10
                         -trial->stock[g*2]-trial->stock[g*2+1]);
        mixes+=need[g];
    }
    if (mixes>STOCK-9) return 0;
    Plan p={0};
    for (int i=0;i<TYPES;i++) p.bag[i]=trial->stock[i];
    for (int g=0;g<3;g++)
        for (int i=0;i<need[g];i++) {
            int a=remaining(&p,(g+1)%3), b=remaining(&p,(g+2)%3);
            if (a<0 || b<0 || !apply(&p,make_move(a,b))) return 0;
        }
    for (int g=0;g<3;g++)
        while (p.value[g]<challenge[trial->weather][g]) {
            int a=remaining(&p,g);
            if (a<0 || !apply(&p,make_move(a,-1))) return 0;
        }
    trial->answer=p;
    return evaluate(p.value,trial->weather).perfect;
}

void meet_customers(Day *day, Plan *plan, int brief) {
    const char *summary[]={
        "ลูกค้าเก่าพาลูกมาซื้อขนม นิดดีใจที่เขายังจำร้านได้",
        "เมฆขายโต๊ะตัวแรกได้ และกำลังคุยวันส่งงานชิ้นต่อไป",
        "อุ่นหาบ้านผู้รับจดหมายเจอแล้ว หลังตามหาอยู่นาน",
        "ฝนว่ายถึงอีกฝั่งได้เป็นครั้งแรก อยากเล่าให้ใครสักคนฟัง",
        "ยายมะลิยังเผลอจัดดอกไม้เผื่อตาที่จากไป",
        "ป่านกังวลกับการสอนครั้งแรก จึงอยากลองซ้อมพูด",
        "ต้นไม้ที่กล้าเล่นใต้ร่มมาตั้งแต่เด็กโค่นลงแล้ว",
        "ไหมจะเดินทางคนเดียวครั้งแรก อยากมีเพื่อนนั่งรอรถ",
        "เต้โกรธที่งานป้ายถูกทาทับโดยไม่มีใครบอกก่อน",
        "ดาวเสียใจที่ไมค์ดับระหว่างเพลงที่ตั้งใจซ้อม",
        "อิฐถูกกล่าวหาว่าทำจานแตก ทั้งที่เพิ่งเข้าครัว",
        "หลิวผิดหวังที่เมล็ดไม่งอก และยังไม่พร้อมเริ่มใหม่"
    };
    const char *help[]={
        "เล่าต่อสิ ฟังอยู่", "ยังมีเรื่องอะไรอีกไหม ดูท่าจะยังเล่าไม่หมด",
        "พักดื่มน้ำก่อน ค่อยเล่าก็ได้", "ตอนแตะขอบสระแล้วเป็นยังไงบ้าง",
        "นั่งด้วยกันก่อนนะยาย", "ลองซ้อมทักทายกับเราก่อนไหม",
        "มีอะไรจากต้นนั้นอยากให้ดูไหม", "ให้นั่งรอรถเป็นเพื่อนไหม",
        "ให้ช่วยดูป้ายที่วาดเมื่อคืนไหม", "เล่าให้จบก่อนก็ได้",
        "เล่าฝั่งของนายให้ฟังหน่อย", "วางถาดพักก่อนดีไหม"
    };

    for (int i=0; i<VISITS; i++) {
        const Story *s=&stories[day->guest[i]];
        heading(s->name);
        printf("  %02d:00  |  ลูกค้าคนที่ %d / %d\n\n",9+i,i+1,VISITS);
        int gift=s->type;
        int picks=day->gift[i];
        int alternate=(gift+2)%TYPES;
        if (brief) {
            printf("  %s\n",summary[day->guest[i]]);
        } else {
            printf("“%s”\n\n",s->story);

            printf("  [1] %s\n  [2] %s\n  [3] %s\n",s->question,s->otherQuestion,help[day->guest[i]]);
            Day trial=*day;
            int offer=0;
            if (i==VISITS-1) {
                trial.stock[gift]--;
                trial.stock[alternate]++;
                offer=solve_trade(&trial);
                if (offer)
                    printf("  [4] คุยต่อและแลก %s 1 ชิ้น เป็น %s (ยังออกดอกได้)\n",
                           crystal[gift],crystal[alternate]);
            }
            int answer=choose(1,offer ? 4 : 3);
            if (answer==4) {
                *day=trial;
                day->changed_gift=1;
            }
            if (inputEnded) return;
            heading("บทสนทนาต่อ");
            if (answer>=3) {
                printf("“%s”\n",s->clarify);
                printf("%s\n", s->listenAction);
            }
            if (answer==2) printf("%s\n", s->otherReply);
            else printf("%s\n", s->reply);
        }
        if (i==VISITS-1 && day->changed_gift) {
            plan->bag[alternate]++;
            picks--;
            printf("  แลกแล้ว - ได้รับผลึก: %s  +1 ชิ้น\n",crystal[alternate]);
        }
        plan->bag[gift]+=picks;
        printf("\n  +--------------------------------------------------+\n");
        if (picks) printf("    ได้รับผลึก: %s  +%d ชิ้น\n",crystal[gift],picks);
        printf("    ในคลังตอนนี้: %s  %d ชิ้น\n",crystal[s->type],plan->bag[s->type]);
        printf("  +--------------------------------------------------+\n");
        if (!brief) {
            printf(i+1<VISITS ? "  กด Enter เพื่อรับลูกค้าคนถัดไป\n"
                              : "  กด Enter เพื่อไปโต๊ะปลูก\n");
            wait_enter();
            if (inputEnded) return;
        }
    }
}

void describe(Move move) {
    if (move.b==-1) printf("%s",crystal[move.a]);
    else printf("%s + %s = %s",crystal[move.a],crystal[move.b],recipe(move.a,move.b));
    printf(" → %s +10\n",part[target(move)]);
}

void show_aunt_ing(void) {
    printf("                  ⣠⢴⣒⣒⣲⢤⡀\n"
         "          ⢀⣀⡤⠤⠤⠤⠒⡞⠒⠲⠤⣌⡹⣽⠘⡆\n"
         "       ⢠⠔⠋⠁⢀⣠⠤⢀⣠⢦⡳⣈⠓⠦⣄⠙⢾⡰⠃\n"
         "      ⣰⠃ ⣠⠞⣉⡤⠖⠋  ⠳⣌⠙⠒⠚⠻⡍⢳\n"
         "      ⡇⢀⣴⠛⠉       ⢀⣙⣲⢤⣀⡀ ⡇\n"
         "     ⢀⡇⡞⢀⡴⠾⠭⠽⢦⡀  ⣠⠝⠒⠚⠻⣆⢉⣡⣽⢤\n"
         "     ⣏⢯⢩⡟⠰⠒⠲⠦ ⢻⢉⢹⡃⠚⠉⠋⠓⢈⡏⢠⠘⡼\n"
         "     ⠸⡌⢸⠙⠦⠤⢤⠤⠴⢫⠏ ⠙⠢⠤⢤⡔⠋ ⡞⣰⠃\n"
         "      ⠿⠛⡆⠈⠙⠂  ⠘⠒⠚ ⢀⡀⠉  ⢸⠛⠟\n"
         "        ⠱⡄  ⠈⠳⣤⠤⢤⡶⠋   ⢠⠃\n"
         "         ⠙⠦⣀  ⠈⠉⠉   ⢀⡴⠁\n"
         "           ⣼⠙⠒⠦⠤⠤⠤⠖⢺⣅\n"
         "     ⢀⡠⠴⠒⠋⢻⡁⠙⠦⣄⣀⣠⠤⠞⠁⣨⠏⠙⠒⠲⢤⣀\n"
         "   ⣠⠞⠉     ⠙⠦⣄⡴⠋⠳⣄⡤⠖⠁     ⠈⠓⢤⡀\n"
         "   ⠁                         ⠁\n");
    printf("ป้าอิงยิ้มให้จากหน้าร้าน\n");
}

void welcome(void) {
    printf("\n");
    printf("  ====================================================\n");
    printf("               Grow a (Feeling) Garden\n");
    printf("                       สวนรับฟัง\n");
    printf("  ====================================================\n\n");
    Plan title_tree={0};
    for (int i=0; i<3; i++) title_tree.value[i]=30;
    title_tree.count=9;
    show_tree(&title_tree,NULL);
    printf("       รับฟังเรื่องราว แล้วปลูกสวนจากความรู้สึก\n\n");
    printf("  ----------------------------------------------------\n");
    printf("    [1]  เริ่มเล่น (เปิดร้าน)\n");
    printf("    [2]  คู่มือเกม\n");
    printf("         รู้จักเกม วิธีเล่น และสูตรผสม\n\n");
    printf("  ----------------------------------------------------\n");
    printf("  พิมพ์หมายเลข แล้วกด Enter\n");
    if (choose(1,2)!=2) return;

    heading("ก่อนเริ่มปลูก");
    printf("  เปิดร้านรับฟังเรื่องราวของลูกค้า\n");
    printf("  เก็บผลึกความรู้สึกมาเลี้ยงต้นไม้ให้ผ่านคืนนี้\n\n");
    printf("  ตอนเช้า - เปิดร้าน\n");
    printf("    ฟังพยากรณ์จากป้าอิง\n");
    printf("    คุยกับลูกค้า แล้วรับผลึกเก็บไว้\n\n");
    printf("  ตอนเย็น - ดูแลต้นไม้\n");
    printf("    เลือกใส่ผลึกตรง ๆ หรือไปหน้าผสมปุ๋ย\n");
    printf("    ย้อนหรือจัดแผนใหม่ได้ ก่อนยืนยันปลูก\n\n");
    printf("\n  [1] อ่านต่อ   [0] เริ่มเล่นเลย\n");
    if (!choose(0,1)) return;
    heading("ดูแลต้นไม้และเป้าหมาย");
    care_notes();
    printf("\n  [1] อ่านต่อ   [0] เริ่มเล่นเลย\n");
    if (!choose(0,1)) return;
    heading("เตรียมรับสภาพอากาศ");
    printf("  แต่ละรอบสุ่มเหตุการณ์หนึ่งแบบ ป้าอิงจะบอกก่อนเปิดร้าน\n");
    printf("  พายุ: ลมทำให้ต้นล้ม ต้องมีรากยึดดินและเปลือกกันกิ่งเสียหาย\n");
    printf("    รากอย่างน้อย 30 และไม่น้อยกว่าค่าใบ\n");
    printf("    เปลือกอย่างน้อย 20 และไม่น้อยกว่าครึ่งหนึ่งของค่าใบ\n\n");
    printf("  แล้ง: ดินแห้ง รากต้องส่งน้ำไปเลี้ยงใบ\n");
    printf("    รากอย่างน้อย 30 และไม่น้อยกว่าค่าใบ\n\n");
    printf("  แมลง: เปลือกช่วยป้องกันต้นไม้จากแมลง\n");
    printf("    เปลือกอย่างน้อย 30 และไม่น้อยกว่าค่าใบ\n\n");
    printf("  ส่วนที่ขาดแต่ละหน่วยทำให้สุขภาพลดลง 2 จากเต็ม 100\n");
    printf("  ถ้าสุขภาพเหลือ 0 ต้นไม้จะไม่รอด\n");
    printf("  เป้าออกดอก: พายุ 20/40/30, แล้ง 30/40/20, แมลง 30/20/40\n");
    printf("\n  [1] อ่านต่อ   [0] เริ่มเล่นเลย\n");
    if (!choose(0,1)) return;
    heading("ทำไมต้องดูสูตรผสม?");
    printf("  บางวันผลึกบางกลุ่มมีไม่พอ\n");
    printf("  เราจึงใช้ผลึกจากอีกสองกลุ่มมาผสมแทน\n\n");
    printf("  ตัวอย่าง: สุข + กลัว = กล้าหาญ\n");
    printf("  ใช้ผลึก 2 ชิ้น แล้วเพิ่มเปลือก 10 หน่วย\n");
    printf("  หน้าถัดไปรวมทุกสูตรไว้ให้ดูว่าแต่ละคู่บำรุงส่วนไหน\n");
    printf("  ตอนเล่นต้องกลับโต๊ะปลูก แล้วเลือกเมนูผสมปุ๋ยเอง\n");
    printf("  เปิดดูสูตรอย่างเดียว ยังไม่เสียผลึก\n\n");
    printf("  [1] ดูสูตรผสม   [0] เริ่มเล่นเลย\n");
    if (!choose(0,1)) return;
    heading("สูตรผสมผลึก");
    for (int a=0; a<TYPES; a++) {
        for (int b=a+1; b<TYPES; b++) {
            if (recipe(a,b)) describe(make_move(a,b));
        }
    }
    printf("\n  [1] เริ่มเนื้อเรื่อง\n");
    if (!choose(1,1)) return;
}

/* เลือกส่วนของต้นไม้ก่อน จะได้เห็นเฉพาะผลึกที่บำรุงส่วนนั้นได้ */
int feed_group(Plan *plan, int group) {
    for (;;) {
        int options[2], count=0;
        printf("\nผลึกสำหรับ%s\n",part[group]);
        for (int i=group*2; i<group*2+2; i++) {
            if (!plan->bag[i]) continue;
            options[count++]=i;
            printf("%d. ใช้%s (เหลือ %d ชิ้น)\n",count,crystal[i],plan->bag[i]);
        }
        if (!count) {
            printf("ผลึกส่วนนี้หมดแล้ว\n");
            return 0;
        }
        printf("เลือกผลึก หรือ 0 เพื่อเลือกส่วนอื่น\n");
        int choice=choose(0,count);
        if (!choice) return 0;
        Move move={options[choice-1],-1};
        int amount=1;
        if (plan->bag[move.a]>1) {
            printf("ใช้กี่ชิ้น? 1–%d หรือ 0 เพื่อเลือกปุ๋ยใหม่\n",plan->bag[move.a]);
            amount=choose(0,plan->bag[move.a]);
            if (inputEnded) return 0;
            if (!amount) continue;
        }
        for (int i=0; i<amount; i++) apply(plan,move);
        printf("เติม%s +%d\n",part[group],10*amount);
        return 1;
    }
}

int feed(Plan *plan) {
    for (;;) {
        printf("\nอยากบำรุงส่วนไหน?\n");
        printf("1. ใบและดอก   2. ราก   3. เปลือก   0. กลับโต๊ะปลูก\n");
        int group=choose(0,3)-1;
        if (group<0) return 0;
        if (feed_group(plan,group)) return 1;
        if (inputEnded) return 0;
    }
}

int mix_fertilizer(Plan *plan) {
    for (;;) {
        int options[TYPES], count=0, groups[3]={0};
        printf("\nโต๊ะผสมปุ๋ย\n");
        for (int i=0; i<TYPES; i++) {
            if (!plan->bag[i]) continue;
            groups[i/2]=1;
            options[count++]=i;
            printf("%d. %s × %d (กลุ่ม%s)\n",count,crystal[i],plan->bag[i],part[i/2]);
        }
        printf("[7] ดูสูตรที่ทำได้ตอนนี้  [0] กลับโต๊ะปลูก\n");
        if (groups[0]+groups[1]+groups[2]<2) {
            printf("ต้องมีผลึกต่างกลุ่มกันสองชิ้นจึงจะผสมได้\n");
        }
        printf("เลือกผลึกชิ้นแรก หรือ 0 เพื่อกลับโต๊ะปลูก\n");
        int first=choose(0,7);
        if (first==7) {
            heading("สูตรที่ใช้ผลึกในคลังได้");
            if (groups[0]+groups[1]+groups[2]<2)
                printf("ยังไม่มีสูตรที่ทำได้\n");
            for (int a=0;a<TYPES;a++)
                for (int b=a+1;b<TYPES;b++)
                    if (a/2!=b/2 && plan->bag[a] && plan->bag[b])
                        describe(make_move(a,b));
            continue;
        }
        if (first>count) {
            printf("ไม่มีผลึกตามเลขนี้\n");
            continue;
        }
        if (!first) return 0;
        printf("เลือกผลึกชิ้นที่สอง หรือ 0 เพื่อกลับโต๊ะปลูก\n");
        int second=choose(0,count);
        if (!second) return 0;
        Move move={options[first-1],options[second-1]};
        if (move.a/2==move.b/2) {
            printf("ผลึกสองชิ้นนี้อยู่กลุ่มเดียวกัน ลองเลือกใหม่ได้\n");
            continue;
        }
        describe(move);
        int limit=smaller(plan->bag[move.a],plan->bag[move.b]);
        printf("ผสมกี่ชุด? 1–%d (0 ยกเลิก) ชุดละ 10 หน่วย\n",limit);
        int amount=choose(0,limit);
        if (!amount) return 0;
        for (int i=0;i<amount;i++) apply(plan,move);
        printf("ผสม %d ชุด เติม%s +%d\n",amount,part[target(move)],10*amount);
        return 1;
    }
}

/* วนรับคำสั่งจนผู้เล่นยืนยันปลูก */
void planting(const Day *day, Plan *plan) {
    int before[STOCK], actions=0;
    while (!inputEnded) {
        heading("โต๊ะปลูก");
        printf("  17:00  |  คืนนี้%s\n\n",weather[day->weather]);
        const int *goal=challenge[day->weather];
        printf("  เป้าออกดอกคืนนี้: ใบ %d / ราก %d / เปลือก %d ขึ้นไป\n",
               goal[0],goal[1],goal[2]);
        for (int i=0;i<3;i++)
            printf("  %s: มี %d / เป้า %d\n",part[i],plan->value[i],goal[i]);
        if (plan->count) {
            Result preview=evaluate(plan->value,day->weather);
            printf("  คาดการณ์: %s | สุขภาพ %d/100 | ออกดอก %s\n",
                   preview.health ? "รอด" : "ไม่รอด",preview.health,
                   preview.health==100 && challenge_met(plan->value,day->weather)
                   ? "ได้" : "ยังไม่ได้");
        }
        Result conditions=evaluate(plan->value,day->weather);
        for (int i=0;i<3;i++) {
            if (plan->value[i]<10)
                printf("  ให้รอด: เติม%sอีก %d ให้ถึง 10\n",part[i],10-plan->value[i]);
            if (conditions.missing[i])
                printf("  ให้สุขภาพเต็ม: เติม%sอีก %d จากแผนตอนนี้\n",
                       part[i],conditions.missing[i]);
        }
        printf("  คลังผลึก\n");
        for (int i=0;i<TYPES;i++)
            printf("    %s %d%s",crystal[i],plan->bag[i],i%2 ? "\n" : " / ");

        printf("\n");
        printf("  [1] ใส่ปุ๋ย\n  [2] ดูหรือแก้แผน\n  [3] ปลูกและปิดร้าน\n  [4] ผสมปุ๋ย\n");
        int action=choose(1,4);
        if (inputEnded) return;
        if (action==1) {
            while (!inputEnded) {
                int previous=plan->count;
                if (!feed(plan)) break;
                before[actions++]=previous;
                printf("  ตอนนี้ ใบ %d / ราก %d / เปลือก %d\n",
                       plan->value[0],plan->value[1],plan->value[2]);
            }
        } else if (action==4) {
            while (!inputEnded) {
                int previous=plan->count;
                if (!mix_fertilizer(plan)) break;
                before[actions++]=previous;
            }
        } else if (action==2) {
            printf("\nต้นไม้ตามแผนตอนนี้\n");
            show_tree(plan,NULL);
            for (int i=0; i<3; i++) {
                int value=plan->value[i];
                printf("%s %d หน่วย · ",part[i],value);
                if (value<goal[i]) printf("อีก %d หน่วยถึงเป้าออกดอก\n",goal[i]-value);
                else printf("ถึงเป้าของส่วนนี้แล้ว\n");
            }
            printf("ผลึกที่เหลือ\n");
            for (int i=0; i<TYPES; i++)
                printf("%s × %d%s",crystal[i],plan->bag[i],i%2 ? "\n" : "   ");
            printf("1. ย้อนครั้งล่าสุด   2. คืนผลึกทั้งหมด   3. เปิดคู่มือ   0. กลับ\n");
            int edit=choose(0,3);
            if (edit==1 && actions) {
                int stop=before[--actions];
                while (plan->count>stop) undo(plan);
                printf("คืนผลึกที่ใส่ครั้งล่าสุดแล้ว\n");
            } else if (edit==1) printf("ยังไม่มีรายการให้ย้อน\n");
            else if (edit==2) {
                while (plan->count) undo(plan);
                actions=0;
                printf("คืนผลึกทั้งหมดแล้ว\n");
            } else if (edit==3) guide(day);
        } else {
            if (!plan->count) printf("เมล็ดนี้ยังไม่ได้ใส่ปุ๋ย\n");
            else for (int i=0;i<3;i++)
                if (plan->value[i]<10)
                    printf("ระวัง: %sยังไม่ถึง 10 ต้นจะไม่รอด\n",part[i]);
            printf("ปิดร้านแล้วจะแก้แผนไม่ได้   1. ยืนยันปลูก   2. กลับไปจัดแผน\n");
            if (choose(1,2)!=2) return;
        }
    }
}

const char *ending(const Plan *p, Result r, int weather_type) {
    if (p->count==0)
        return "วันนี้ยังไม่ได้ใช้ผลึก เมล็ดเลยยังไม่โตนะลูก\n"
               "  รอบหน้าลองเริ่มใส่ทีละส่วนก็ได้ ถ้าไม่พอค่อยดูว่าจะผสมอะไรเพิ่ม";
    if (p->value[0]<10 || p->value[1]<10 || p->value[2]<10)
        return "มีส่วนที่ยังไม่ได้รับปุ๋ยเลย รอบหน้าแบ่งให้ครบทั้งสามส่วนก่อนนะ\n"
               "  ป้าก็เคยมัวแต่ดูยอดจนลืมดูราก บางอย่างไม่เด่น แต่ขาดไปก็ลำบากเหมือนกัน";
    if (r.perfect)
        return "ออกดอกแล้วนะลูก ทั้งใบ ราก และเปลือกได้โตไปด้วยกัน\n"
               "  คนเราก็มีหลายความรู้สึก วันที่ดีทำให้มีกำลังใจ\n"
               "  ส่วนวันที่ยาก ถ้าได้พักและมีคนช่วย ก็อาจทำให้เราได้เรียนรู้อีกอย่าง";
    if (r.health==0) {
        if (weather_type==2)
            return "คราวนี้เปลือกยังป้องกันแมลงไม่พอ รอบหน้าลองเผื่อผลึกไว้ให้เปลือกด้วย\n"
                   "  ความโกรธก็มีประโยชน์นะลูก บางครั้งมันบอกว่ามีเรื่องที่เราไม่ควรยอมทน";
        if (weather_type==1)
            return "รากยังส่งน้ำให้ใบไม่พอ รอบหน้าลองเพิ่มรากก่อนเพิ่มใบ\n"
                   "  วันที่เคยทำได้ดี ไม่ได้แปลว่าเราต้องรับมือไหวทุกครั้ง ขอให้คนอื่นช่วยบ้างก็ได้";
        return "พายุครั้งนี้แรงเกินกว่าต้นจะรับไหว รอบหน้าดูทั้งรากและเปลือกนะ\n"
               "  พลาดครั้งนี้ยังลองใหม่ได้ ค่อยดูว่าอะไรขาด แล้วแก้ทีละอย่าง";
    }
    if (r.missing[1] && r.missing[2])
        return "ทั้งรากและเปลือกยังไม่พอรับพายุ รอบหน้าเผื่อให้สองส่วนนี้ด้วยนะ\n"
               "  เวลาเจอเรื่องหนัก ๆ เราก็ต้องดูแลตัวเองหลายด้าน จะพึ่งอย่างเดียวคงไม่ไหว";
    if (r.missing[2])
        return "เปลือกยังบางไปหน่อย แต่ต้นยังรอด คราวหน้าลองเพิ่มให้ตรงนี้นะ\n"
               "  เวลามีใครทำให้เราเจ็บ บอกเขาบ้างก็ได้ ไม่ต้องยอมไปทุกเรื่อง";
    if (r.missing[1])
        return "รากยังเลี้ยงยอดได้ไม่เต็มที่ รอบหน้าลองเผื่อผลึกให้รากก่อน\n"
               "  เวลากลัว ลองดูว่าเรายังไม่พร้อมตรงไหน แล้วค่อยหาคนช่วยตรงนั้น";
    return "ต้นนี้ผ่านคืนนี้มาได้ดีแล้ว แค่ยังโตไม่ครบตามเงื่อนไขออกดอก\n"
           "  วันนี้ยังไม่ถึงเป้าก็ไม่ได้แปลว่าที่ทำมาสูญเปล่า เก็บสิ่งที่ได้ลองไปใช้รอบหน้านะ";
}

/* ผลึกชนิดเดียวกันใช้ของลูกค้าที่มาก่อน เลือกเล่าถึงคนที่ฝาก
   วัตถุดิบของสูตรแรก ถ้าไม่ได้ผสมให้เล่าถึงผลึกชิ้นแรกแทน */
void visitor_returns(const Day *day, const Plan *p) {
    if (!p->count) return;
    int index=0;
    for (int i=0; i<p->count; i++) {
        if (p->moves[i].b>=0) { index=i; break; }
    }
    Move move=p->moves[index];
    int type=move.a, used=0;
    for (int i=0; i<index; i++) {
        if (p->moves[i].a==type || p->moves[i].b==type) used++;
    }
    for (int i=0; i<VISITS; i++) {
        int id=day->guest[i];
        if (i==VISITS-1 && day->changed_gift &&
            (stories[id].type+2)%TYPES==type && used==0) {
            heading("ลูกค้าแวะมาอีกครั้ง");
            printf("  %s เล่าว่า\n",stories[id].name);
            printf("“%s”\n",stories[id].nextMorning);
            printf("ผลึก%sที่แลกกันเมื่อวานถูกใช้กับต้นนี้ด้วย\n",crystal[type]);
            break;
        }
        if (stories[id].type!=type) continue;
        int quantity=day->gift[i];
        if (i==VISITS-1 && day->changed_gift) quantity--;
        if (used>=quantity) { used-=quantity; continue; }
        heading("ลูกค้าแวะมาอีกครั้ง");
        printf("  %s เล่าว่า\n",stories[id].name);
        printf("  “%s”\n\n",stories[id].nextMorning);
        printf("  ผลึก%sที่เขาให้เมื่อวาน ",crystal[type]);
        if (move.b>=0) printf("ผสมเป็น “%s” ",recipe(move.a,move.b));
        printf("ได้เลี้ยง%sของต้นนี้\n",part[target(move)]);

        break;
    }
}

void morning(const Day *day,const Plan *p) {
    const char *event[]={"ฝนสาดเข้าที่กิ่ง ยอดเอนตามแรงลม","ลมร้อนพัดจนผิวดินเริ่มแตกระแหง","แมลงไต่จากโคนต้นขึ้นไปหายอดอ่อน"};
    Result r=evaluate(p->value,day->weather);
    printf("\nกลางคืน  |  %s\n%s\n",weather[day->weather],event[day->weather]);
    printf("1. รอจนฟ้าสาง\n");
    if (!choose(1,1)) return;
    printf("\nเช้าวันถัดมา\n"); show_tree(p,&r);
    heading("ผลการปลูก");
    if (!p->count) printf("  ยังไม่ได้ใส่ปุ๋ย - เมล็ดยังไม่เติบโต\n");
    else if (r.perfect) printf("  Perfect - ต้นไม้รอดและออกดอก\n");
    else if (!r.health) printf("  ต้นไม้ไม่รอด\n");
    else printf("  ต้นไม้รอด แต่ยังไม่ออกดอก\n");
    printf("  ใบ %d / ราก %d / เปลือก %d\n",p->value[0],p->value[1],p->value[2]);
    if (p->count) {
        printf("  คืนที่ผ่านมา: %s | สุขภาพเหลือ %d/100\n",weather[day->weather],r.health);
        for (int i=0;i<3;i++)
            if (p->value[i]<10)
                printf("  %sมี %d ต้องอย่างน้อย 10 จึงจะอยู่รอด\n",part[i],p->value[i]);
        if (r.missing[1]) printf("  รากขาดอีก %d หน่วย จึงรับมือคืนนี้ได้ไม่เต็มที่\n",r.missing[1]);
        if (r.missing[2]) printf("  เปลือกขาดอีก %d หน่วย จึงรับมือคืนนี้ได้ไม่เต็มที่\n",r.missing[2]);
        if (!r.perfect && r.health==100)
            printf("  ยังไม่ออกดอก เพราะปุ๋ยยังไม่ถึงเป้าของคืนนี้\n");
    }
    printf("\n  คะแนนรวม %.0f/100%s\n",r.growth+r.balance+r.weatherScore,
           r.perfect ? "   Perfect!" : "");
    printf("  เติบโต %.1f/30 | ตามเป้า %.1f/40 | สุขภาพ %.1f/30\n",
           r.growth,r.balance,r.weatherScore);
    visitor_returns(day,p);
    heading("ป้าอิงฝากไว้");
    printf("  “%s”\n",ending(p,r,day->weather));
    printf("\n  ----------------------------------------------------\n");

}

/* จุดเริ่มเกมและลำดับของแต่ละรอบ */
int main(void) {
#ifdef _WIN32
    system("chcp 65001 >nul"); /* ให้คอนโซล Windows อ่านข้อความเป็น UTF-8 */
#endif
    srand((unsigned)time(NULL));
    welcome();
    while (!inputEnded) {
        Day day;
        Plan plan={0};
        new_day(&day);
        heading("Grow a (Feeling) Garden");
        printf("  08:00  |  ป้าอิงวางห่อกระดาษลงบนเคาน์เตอร์\n\n");
        show_aunt_ing();
        printf("“ฝากเมล็ดไว้สักคืนนะ วันนี้ใครแวะมาก็ลองฟังเขาดู พอตกเย็นค่อยเอาผลึกที่เขาฝากไว้มาปลูกกัน”\n");
        printf("ป้าอิงวางสมุดดูแลเมล็ดไว้ข้างห่อกระดาษ แล้วชี้ให้ดูพยากรณ์คืนนี้\n");
        printf("%s\n", forecast[day.weather]);
        printf("[1] เปิดร้าน อ่านบทเต็ม\n[2] เปิดร้าน อ่านแบบย่อ (ผลึกเท่าเดิม)\n");
        int mode=choose(1,2);
        if (inputEnded) return 0;
        meet_customers(&day,&plan,mode==2);
        if (inputEnded) return 0;
        planting(&day,&plan);
        if (inputEnded) return 0;
        grow_tree(&plan);
        if (inputEnded) return 0;
        morning(&day,&plan);
        if (inputEnded) return 0;
        printf("1. ดูสูตร Perfect ของคืนนี้   2. เล่นรอบใหม่   3. ออกจากเกม\n");
        int action=choose(1,3);
        if (action==1) {
            printf("เริ่มจากคลังก่อนใส่ปุ๋ย แล้วทำตามนี้ทีละบรรทัด\n");
            for (int i=0;i<day.answer.count;i++) describe(day.answer.moves[i]);
            printf("2. เล่นรอบใหม่   3. ออกจากเกม\n");
            action=choose(2,3);
        }
        if (action==3) return 0;
    }
    return 0;
}
