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
     "เมื่อเช้ามีคนเรียกป้านิด ๆ หันไปนึกว่าใคร เจ้าเด็กที่เคยซื้อขนมก่อนไปโรงเรียนนั่นเอง เดี๋ยวนี้อุ้มลูกมาแล้ว ฉันนี่จำหน้าแทบไม่ได้",
     "ขออวดหน่อยเถอะ ยืนยิ้มคนเดียวมาตั้งแต่เช้าแล้ว",
     "เขายังถามหาชิ้นริมอยู่เลยนะ ฉันก็ว่าเอ๊ะ ยังจำได้อีกเหรอ ลูกเขาเห็นพ่อกินก็จะเอาบ้าง เลยได้แถมไปสองชิ้น",
     "เขายังซื้อขนมแบบเดิมอยู่ไหม", "เมื่อก่อนเขามาซื้อบ่อยเหรอ",
     "แทบทุกเช้าเลย ชอบมาถึงตอนฉันยังตัดไม่เสร็จ ยืนมองมีดอยู่นั่นแหละ บอกให้ไปนั่งก็ไม่ไป",
     "วันนี้เขาไม่ได้แวะนะ แต่ฉันยังตัดชิ้นริมเผื่อไว้อยู่เลย",
     "คุณนั่งฟัง นิดหัวเราะแล้วเล่าต่อ"},
    {1, "เมฆ · ช่างไม้",
     "โต๊ะขายออกแล้วครับ ตัวที่ผมทำอยู่นานนั่นแหละ ตอนเขาจ่ายเงินผมนับอยู่สองรอบ ไม่ใช่อะไรนะ ไม่คิดว่าจะมีคนซื้อจริง ๆ",
     "ขอเล่าเรื่องลูกค้าหน่อยครับ ยังไม่ได้เล่าให้ใครฟังเลย",
     "เขาถามด้วยว่ารับทำอีกไหม ผมตอบรับไปแล้ว ค่อยกลับมานั่งคิดว่าจะทำทันหรือเปล่า สงสัยต้องโทรไปคุยวันส่งใหม่",
     "ลูกค้าได้ลองนั่งไหม เขาว่ายังไงบ้าง", "ทำส่วนไหนนานที่สุด",
     "ขาโต๊ะครับ ตอนแรกวางแล้วโยก ผมไสไปไสมาจนเตี้ยกว่าที่วาดไว้ ต้องแก้ใหม่หมด ตอนนี้ลองโยกยังไงก็ไม่ขยับแล้ว",
     "โทรคุยวันส่งแล้วครับ เขารอได้ ค่อยโล่งหน่อย",
     "เมฆเลื่อนแก้วหลบ แล้วกางมือให้ดูขนาดโต๊ะ"},
    {0, "อุ่น · คนส่งจดหมาย",
     "ขอน้ำหน่อยครับ เดินเลยร้านไปแล้วเพิ่งนึกได้ว่ายังไม่ได้กินอะไรเลย วันนี้หาบ้านผู้รับเจอสักที ที่อยู่บนซองใช้เลขบ้านเก่า ผมวนอยู่หลายรอบ",
     "ขอนั่งแป๊บหนึ่งครับ เดี๋ยวเล่าให้ฟัง กว่าจะเจอนี่ถามไปครึ่งซอย",
     "ได้ป้าร้านชำช่วยบอกทางครับ ผมเคยเดินผ่านบ้านนั้นด้วยนะ แต่มัวแต่หาเลขบนซอง เขาออกมารับผมเลยถามชื่อซ้ำตั้งสองที กลัวผิดบ้านอีก",
     "สุดท้ายหาเจอได้ยังไง", "แล้วเจ้าของจดหมายว่าไงบ้าง",
     "เขาบอกว่ารออยู่พอดีครับ หายเข้าไปหยิบแว่นแล้วกลับมาอ่านตรงประตูเลย ผมก็อยากรู้ว่าในนั้นเขียนอะไร แต่ต้องไปส่งบ้านอื่นต่อแล้ว",
     "วันนี้ผ่านร้านชำ เลยแวะขอบคุณป้าแกหน่อย ไม่งั้นคงหาไม่เจอ",
     "คุณวางน้ำให้อุ่น เขาดื่มไปหลายอึกก่อนเล่าต่อ"},
    {1, "ฝน · นักว่ายน้ำ",
     "ดูแขนสิ ยกแทบไม่ขึ้นเลย เมื่อวานว่ายถึงอีกฝั่งแล้วนะ ไม่ได้เกาะเชือกด้วย ตอนกลับบ้านอยากเล่าให้แม่ฟัง แต่แม่หลับไปแล้ว",
     "ขอเล่าอีกทีนะ เล่าแล้วก็ยังดีใจอยู่",
     "ครูบอกให้พักก่อน แต่ฉันเกาะขอบแล้วหันกลับไปมองฝั่งที่เพิ่งว่ายมา มันก็ไม่ได้ไกลขนาดนั้นนี่นา ตอนอยู่กลางสระนึกว่าจะไม่ถึงแล้ว",
     "พอถึงอีกฝั่ง ทำอะไรก่อนเลย", "ตอนอยู่กลางสระกลัวไหม",
     "กลัวสิ เผลอกลืนน้ำไปทีหนึ่งด้วย เกือบจะคว้าเชือกแล้ว แต่ได้ยินครูบอกว่าอีกนิดเดียว ก็เลยลองต่อ",
     "วันนี้เล่าให้แม่ฟังแล้ว แม่ถามว่ามีใครถ่ายคลิปไว้ไหม เสียดายเหมือนกัน",
     "ฝนวางกระเป๋าลงข้างเก้าอี้ แล้วเริ่มเล่าตั้งแต่ตอนลงสระ"},
    {2, "ยายมะลิ · คนจัดดอกไม้",
     "ฝากดอกไม้ไว้หน่อยได้ไหมลูก ยายมัดมาเกินอีกแล้ว เคยทำไว้ให้ตาแกถือกลับบ้าน พอตาไม่อยู่ มือก็ยังหยิบมาเท่าเดิมทุกที",
     "ขอนั่งด้วยสักพักนะ ยังไม่อยากกลับบ้านตอนนี้",
     "มีแจกันไหม เอากำนี้ใส่น้ำไว้เถอะ ก้านมันเริ่มเหี่ยวแล้ว อีกกำยายเอากลับเอง",
     "เอาดอกไม้ใส่น้ำก่อนไหมยาย", "เมื่อก่อนตามาช่วยจัดด้วยไหม",
     "ช่วยแกะยางน่ะสิ ให้จัดทีไรชอบยัดแน่นจนดอกช้ำ ยายบ่นแกประจำ พอไม่มีคนทำให้บ่นก็เงียบไปอีก",
     "ยายเอายางมาคืน เมื่อวานติดกลับไปด้วย ดอกไม้ยังอยู่ดีไหมลูก",
     "คุณเลื่อนเก้าอี้ให้ยาย แล้วหยิบแจกันมาวางบนโต๊ะ"},
    {3, "ป่าน · ครูฝึกสอน",
     "ขอนั่งซ้อมพูดตรงนี้ได้ไหมคะ พรุ่งนี้สอนเองครั้งแรก เมื่อกี้ลองพูดหน้ากระจกแล้วลืมแม้แต่ชื่อตัวเอง ขนาดยังไม่มีเด็กนั่งฟังนะ",
     "ลองเป็นนักเรียนให้หน่อยได้ไหมคะ แค่ตอนทักทายก็พอ",
     "งั้นถามชื่อทีละคนก่อนเนอะ ฉันจะได้มีเวลาหายใจบ้าง แต่ถ้าเด็กถามกลับว่าครูตื่นเต้นเหรอ คงต้องบอกว่ามากเลยค่ะ",
     "เริ่มจากให้เด็ก ๆ แนะนำตัวดีไหม", "กลัวอะไรที่สุดตอนเข้าห้อง",
     "กลัวถามแล้วไม่มีใครตอบค่ะ ต้องยืนยิ้มอยู่คนเดียว เคยเห็นครูพี่เลี้ยงรอได้ตั้งนาน ฉันคงรีบตอบเองหมด",
     "เด็กถามจริง ๆ ว่าครูตื่นเต้นเหรอ ฉันบอกว่าใช่ เขาก็หัวเราะกัน แล้วก็มีคนยกมือช่วยตอบ",
     "คุณนั่งหันหน้าเข้าหาป่าน ให้เธอลองทักทายเหมือนอยู่หน้าห้อง"},
    {2, "กล้า · คนดูแลสนาม",
     "ต้นหูกวางข้างสนามล้มแล้วครับ เมื่อเช้าช่วยเขาเก็บกิ่งอยู่ ตรงนั้นโล่งจนผมเดินผ่านแล้วไม่ชิน ตอนเด็กเล่นอยู่ใต้ต้นนั้นแทบทุกวัน",
     "ขอเล่าเรื่องต้นนั้นหน่อยครับ กลับไปที่สนามก็คงต้องเก็บต่อ",
     "ตอนโกยดินเจอลูกแก้วด้วย ไม่รู้ของผมหรือเปล่า สมัยก่อนซ่อนกันไว้ตามราก ตอนนี้ล้างแล้วใส่กระเป๋าไว้ เดี๋ยวเอาให้น้องดู",
     "ตอนเก็บกิ่งเจอของเก่าบ้างไหม", "เมื่อก่อนเล่นอะไรตรงนั้น",
     "เล่นลูกแก้วกับน้องครับ แพ้ทีไรก็หาว่ามันโกง สุดท้ายแม่ต้องมาตามกลับบ้านทั้งคู่ จำได้ว่ากลับไปโดนดุเรื่องเสื้อเปื้อนอีก",
     "น้องบอกจำลูกแก้วไม่ได้แล้ว แต่ขอเก็บไว้ ผมก็ให้มันไป",
     "คุณรอให้กล้าวางถุงมือ เขาล้วงของเล็ก ๆ ออกจากกระเป๋า"},
    {3, "ไหม · นักเดินทาง",
     "รถยังไม่มาหรอกใช่ไหมคะ ฉันดูเวลาไปสามรอบแล้ว นี่เดินทางคนเดียวครั้งแรก แม่โทรมาถามก็บอกว่าสบายมาก แต่ตอนนี้อยากให้แม่มาส่งจัง",
     "นั่งเป็นเพื่อนจนรถมาหน่อยได้ไหมคะ กลัวมัวแต่มองอย่างอื่นแล้วขึ้นไม่ทัน",
     "ขอบคุณนะคะ ตั๋วก็อยู่นี่ กระเป๋าก็ครบ เหลือแต่ขึ้นให้ถูกคันนี่แหละ ถ้าเห็นรถมาก่อนช่วยสะกิดด้วยนะ",
     "รอตรงนี้ด้วยกันไหม มองเห็นป้ายพอดี", "ไปถึงแล้วมีใครมารับไหม",
     "มีเพื่อนมารับค่ะ ส่งรูปทางออกมาให้แล้ว ฉันบันทึกไว้หมดเลย แต่ก็ยังกลัวเดินออกผิดฝั่งอยู่ดี",
     "ถึงแล้วนะคะ ออกผิดฝั่งจริงด้วย แต่โทรหาเพื่อนแล้วเจอกันแล้ว",
     "คุณขยับเก้าอี้ให้ไหมนั่งตรงที่มองเห็นป้ายรถ"},
    {4, "เต้ · ช่างป้าย",
     "ผมวาดป้ายจนดึก พอไปถึงร้านเช้านี้ เขาทาสีทับไปแล้วครับ บอกว่าเจ้าของอยากได้อีกแบบ แล้วทำไมไม่โทรมาบอกก่อน ผมถือพู่กันยืนอยู่ตั้งนาน",
     "ขอบ่นก่อนนะครับ ตอนนี้ถ้าไปคุยคงเสียงดังแน่",
     "ผมอยากถามแค่ว่ารู้ตั้งแต่เมื่อไหร่ ถ้ารู้เมื่อวาน ผมจะได้ไม่ต้องนั่งทำจนดึก เรื่องแบบแก้ได้ แต่ช่วยบอกกันก่อน",
     "ถ้าได้คุย อยากถามเขาเรื่องไหนก่อน", "เขาทาทับหมดเลยเหรอ",
     "เหลือขอบนิดเดียวครับ ตรงตัวหนังสือหายหมด ผมถ่ายรูปไว้เมื่อคืน ยังดีที่ถ่าย ไม่งั้นไม่มีอะไรให้ดูแล้ว",
     "ยังไม่ได้คุยยาวครับ วันนี้เขารีบ ผมเลยนัดเวลาไว้ก่อน ไม่อยากคุยกันตอนเดินผ่าน",
     "คุณฟังเต้บ่น เขาหยิบโทรศัพท์มาเปิดรูปป้ายเมื่อคืน"},
    {5, "ดาว · นักร้อง",
     "เมื่อคืนไมค์ดับค่ะ ตรงท่อนที่ซ้อมมาเยอะที่สุดเลย ตอนลงเวทีก็ยิ้มให้คนอื่นนะ แต่กลับบ้านแล้วเปิดคลิปดูไม่ได้สักที",
     "ขอเล่าให้จบก่อนได้ไหม ยังไม่อยากได้ยินว่าคราวหน้าค่อยเอาใหม่",
     "ได้ยินคนข้างหน้าร้องต่ออยู่ค่ะ ฉันก็ร้องตามจนจบ แต่ตอนนั้นในหัวคิดแต่ว่าทำไมต้องดับตอนนี้ด้วย",
     "แล้วตอนนั้นร้องต่อจนจบไหม", "เสียดายท่อนไหนที่สุด",
     "ท่อนที่แม่ชอบค่ะ ชวนแม่มาฟังด้วย ตอนกลับแม่บอกว่าสนุก แต่ฉันยังหงุดหงิดจนตอบแม่ไปนิดเดียว รู้สึกผิดอยู่เหมือนกัน",
     "ยังไม่ได้ดูคลิปนะ แต่โทรคุยกับแม่แล้ว แม่บอกได้ยินฉันร้องอยู่ แค่นั่งไกลไปหน่อย",
     "คุณวางแก้วลงแล้วฟังต่อ ดาวหยุดนิดหนึ่งก่อนเล่าถึงตอนอยู่บนเวที"},
    {4, "อิฐ · คนล้างจาน",
     "ผมเพิ่งเข้าครัว เขาก็บอกว่าทำจานแตกอีกแล้ว ผ้ายังไม่ได้ผูกเลยครับ จะให้ผมทำแตกตอนไหน พอจะอธิบายก็บอกให้รีบเก็บก่อน",
     "ฟังผมก่อนนะครับ ผมเล่าเรื่องนี้ให้ใครฟังก็มีแต่คนบอกว่าช่างมัน",
     "ตอนนั้นมีคนอยู่หน้าอ่างสองคน ผมยังไม่ได้เดินไปถึงเลย อยากบอกแค่ว่าเห็นหรือเปล่าว่าผมทำ ถ้าไม่เห็นก็อย่าเพิ่งว่ากัน",
     "ตอนเดินเข้าไป มีใครอยู่บ้าง", "แล้วได้คุยกันอีกรอบไหม",
     "ยังครับ ลูกค้าเข้าพอดี ทุกคนก็แยกไปทำงาน ผมเลยยังคาใจอยู่ เขาอาจลืมไปแล้วด้วยซ้ำ",
     "ได้บอกแล้วครับว่าเมื่อวานผมไม่ได้ทำ เขาตอบแค่ว่าอ๋อ ผมก็ยังหงุดหงิดอยู่นะ",
     "คุณให้อิฐเล่าตั้งแต่เดินเข้าครัว โดยยังไม่ขัด"},
    {5, "หลิว · คนเพาะกล้า",
     "ช่วยดูถาดนี้หน่อยสิ รอมาตั้งหลายวัน ไม่ขึ้นสักต้น คนอื่นหว่านทีหลังใบออกกันแล้ว ฉันเดินไปดูทุกเช้าจนไม่อยากดูแล้ว",
     "ขอวางไว้ตรงนี้ก่อนนะ หิ้วมาแล้วก็ยังไม่รู้จะทำยังไงต่อ",
     "วันนี้คงยังไม่หว่านใหม่หรอก เดี๋ยวเอาถาดนี้กลับไปดูก่อนว่าข้างล่างแฉะไหม เมื่อวานฉันก็เพิ่งรดน้ำเพิ่มอีก",
     "จะลองหว่านใหม่เลยไหม หรือพักก่อน", "รดน้ำไปเยอะไหม",
     "อาจจะเยอะนะ เห็นหน้าดินแห้งก็เติม ไม่ได้ดูข้างล่างเลย พูดแล้วอยากกลับไปยกถาดดู แต่ถ้าเน่าหมดก็คงเสียดายอีก",
     "ยกถาดดูแล้ว น้ำขังจริง ๆ ยังไม่ได้หว่านใหม่ ขอเอาถาดไปผึ่งก่อน",
     "คุณช่วยหลิววางถาดบนพื้น เธอเช็ดมือแล้วนั่งลง"}
};

/* ผลึก คะแนน และการสุ่มวัน */
const char crystal[][64] = {"สุข","ภูมิใจ","เศร้า","กลัว","โกรธ","ผิดหวัง"};
const char part[][64] = {"ใบและดอก","ราก","เปลือก"};
const char weather[][64] = {"พายุ","แล้ง","แมลง"};
const char forecast[][512] = {
 "คืนนี้มีพายุ • รากอย่างน้อย 30 และไม่น้อยกว่าใบ ส่วนเปลือกต้องถึง 20 และอย่างน้อยครึ่งหนึ่งของใบ",
 "คืนนี้ดินจะแห้ง • รากต้องถึง 30 และไม่น้อยกว่าใบ จึงจะส่งน้ำไปเลี้ยงยอดได้ทั่ว",
 "คืนนี้แมลงจะลง • เปลือกต้องถึง 30 และไม่น้อยกว่าใบ จึงจะป้องกันยอดอ่อนได้"};
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

Result evaluate(const int v[3],int w) {
 Result r={0};
 int low=v[0],high=v[0];
 for(int i=0;i<3;i++) {
  low=smaller(low,v[i]); high=larger(high,v[i]);
  r.growth+=smaller(v[i],30)/3.0;
 }
 int rootGoal = 0, barkGoal = 0;
 if (w == 0 || w == 1) rootGoal = larger(30, v[0]);
 if (w == 0) barkGoal = larger(20, v[0] / 2);
 if (w == 2) barkGoal = larger(30, v[0]);
 r.missing[1] = larger(0, rootGoal - v[1]);
 r.missing[2] = larger(0, barkGoal - v[2]);
 r.health=larger(0,100-2*(r.missing[1]+r.missing[2]));
 r.weatherScore=.3*r.health;
 if (high > 0) r.balance = 40.0 * low / high;
 r.perfect=low>=30 && low==high && r.health==100;
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
    while (p.value[g]<30) {
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
        int scarce=draw(3), left[3]={3,3,3};
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
    if (frame>=4 && p->value[0]>=30) image=5;
    if (frame>=2 && p->value[0]==0) image=3;
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

void grow_tree(const Plan *p) {
    printf("\nสวนหลังร้าน\n");
    if (!p->count) {
        draw_frame(p,0,NULL);
        return;
    }
    char line[100];
    for (int frame=0; frame<=4; frame++) {
        draw_frame(p,frame,NULL);
        if (frame<4) {
            printf("กด Enter เพื่อดูต้นไม้โตต่อ\n");
            if (!fgets(line,sizeof line,stdin)) {
                inputEnded=1;
                return;
            }
        }
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
        if (sscanf(line, "%d %c", &answer, &extra) == 1 &&
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
    printf("  ถ้าอยากให้ออกดอก\n");
    printf("    เลี้ยงทั้งสามส่วนให้เท่ากัน\n");
    printf("    แต่ละส่วนต้องได้อย่างน้อย 30 หน่วย\n");

}

void guide(const Day *day) {
    printf("\nสมุดที่ป้าอิงฝากไว้\n");
    care_notes();
    printf("%s\n", forecast[day->weather]);
}

void meet_customers(const Day *day, Plan *plan) {

    for (int i=0; i<VISITS; i++) {
        const Story *s=&stories[day->guest[i]];
        heading(s->name);
        printf("  %02d:00  |  ลูกค้าคนที่ %d / %d\n\n",9+i,i+1,VISITS);
        printf("“%s”\n\n",s->story);

        printf("  [1] %s\n  [2] %s\n  [3] ถามว่าอยากให้ช่วยอะไร\n",s->question,s->otherQuestion);
        int answer=choose(1,3);
        if (inputEnded) return;
        if (answer==3) {
            printf("“%s”\n",s->clarify);
            printf("%s\n", s->listenAction);
        }
        if (answer==2) printf("%s\n", s->otherReply);
        else printf("%s\n", s->reply);
        plan->bag[s->type]+=day->gift[i];
        printf("รับผลึก%s %d ชิ้น เก็บเข้าคลังแล้ว\n",crystal[s->type],day->gift[i]);
    }
}

void proportions(const Plan *p) {
    int total=p->value[0]+p->value[1]+p->value[2];
    if (total==0) { printf("ยังไม่ได้ใส่ปุ๋ย\n"); return; }
    for (int i=0; i<3; i++)
        printf("%s %d (%.1f%%)   ",part[i],p->value[i],100.0*p->value[i]/total);
    printf("\n");
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
    printf("    [1]  เปิดร้าน\n");
    printf("         เริ่มเรื่องกับป้าอิง\n");
    printf("    [2]  ก่อนเริ่มปลูก\n");
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
    care_notes();
    printf("\n1. เปิดหน้าสูตรผสม\n");
    if (!choose(1,1)) return;
    heading("สูตรผสมผลึก");
    for (int a=0; a<TYPES; a++) {
        for (int b=a+1; b<TYPES; b++) {
            if (recipe(a,b)) describe(make_move(a,b));
        }
    }
    printf("\n1. เริ่มเนื้อเรื่อง\n");
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
    int options[TYPES], count=0, groups[3]={0};
    printf("\nโต๊ะผสมปุ๋ย\n");
    for (int i=0; i<TYPES; i++) {
        if (!plan->bag[i]) continue;
        groups[i/2]=1;
        options[count++]=i;
        printf("%d. %s × %d\n",count,crystal[i],plan->bag[i]);
    }
    if (groups[0]+groups[1]+groups[2]<2) {
        printf("ต้องมีผลึกต่างกลุ่มกันสองชิ้นจึงจะผสมได้\n");
        return 0;
    }
    printf("เลือกผลึกชิ้นแรก หรือ 0 เพื่อกลับโต๊ะปลูก\n");
    int first=choose(0,count);
    if (!first) return 0;
    printf("เลือกผลึกชิ้นที่สอง หรือ 0 เพื่อกลับโต๊ะปลูก\n");
    int second=choose(0,count);
    if (!second) return 0;
    Move move={options[first-1],options[second-1]};
    if (move.a/2==move.b/2) {
        printf("ผลึกสองชิ้นนี้อยู่กลุ่มเดียวกัน จึงผสมกันไม่ได้\n");
        return 0;
    }
    describe(move);
    printf("1. ผสมแล้วใส่ลงต้นไม้   0. กลับโต๊ะปลูก\n");
    if (!choose(0,1)) return 0;
    return apply(plan,move);
}

void planting(const Day *day, Plan *plan) {
    int before[STOCK], actions=0;
    while (!inputEnded) {
        heading("โต๊ะปลูก");
        printf("  17:00  |  คืนนี้%s\n\n",weather[day->weather]);
        printf("อยากให้ต้นออกดอก เลี้ยงทั้งสามส่วนให้เท่ากัน ด้านละอย่างน้อย 30\n");
        printf("\n  ใบ %d   |   ราก %d   |   เปลือก %d\n\n",plan->value[0],plan->value[1],plan->value[2]);
        printf("  [1] ใส่ปุ๋ย\n  [2] ดูหรือแก้แผน\n  [3] ปลูกและปิดร้าน\n  [4] ผสมปุ๋ย\n");
        int action=choose(1,4);
        if (inputEnded) return;
        if (action==1) {
            int previous=plan->count;
            if (feed(plan)) before[actions++]=previous;
        } else if (action==4) {
            int previous=plan->count;
            if (mix_fertilizer(plan)) before[actions++]=previous;
        } else if (action==2) {
            printf("\nต้นไม้ตามแผนตอนนี้\n");
            show_tree(plan,NULL);
            for (int i=0; i<3; i++) {
                int value=plan->value[i];
                printf("%s %d หน่วย · ",part[i],value);
                if (value<30) printf("อีก %d หน่วยถึงขั้นต่ำ\n",30-value);
                else printf("ถึงขั้นต่ำแล้ว\n");
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
            printf("ปิดร้านแล้วจะแก้แผนไม่ได้   1. ยืนยันปลูก   2. กลับไปจัดแผน\n");
            if (choose(1,2)!=2) return;
        }
    }
}

const char *ending(const Plan *p, Result r, int weather_type) {
    if (p->count==0)
        return "เมล็ดยังอยู่ตรงที่เราฝังไว้เลยลูก ผลึกเมื่อวานก็ยังอยู่ครบ\n"
               "เมื่อวานหนูฟังคนอื่นมาตั้งหลายคน เช้านี้ลองฟังตัวเองดูบ้าง ถ้าเลือกฝังได้ชิ้นเดียว หนูอยากเริ่มจากความรู้สึกไหนล่ะ";
    if (r.perfect)
        return "ดูสิ เช้านี้มีดอกแล้ว เรามองเห็นดอกก่อนเพื่อนเลย ทั้งที่รากก็โตมาด้วยกันใต้ดิน\n"
               "ป้าเองก็เหมือนกันลูก เวลาพูดถึงวันที่มีความสุข ป้ามักลืมเล่าถึงวันที่ร้องไห้แล้วมีคนมานั่งข้าง ๆ วันนั้นก็มีส่วนให้ป้าโตมาเป็นคนที่นั่งฟังหนูอยู่วันนี้นะ";
    if (r.health==0) {
        if (weather_type==2)
            return "ต้นนี้ไปไม่รอดแล้วลูก ดูรอยที่โคนสิ แมลงเจาะผ่านเปลือกเข้าไปถึงเนื้อไม้แล้ว\n"
                   "เมื่อก่อนป้าโกรธใครก็ได้แต่ยิ้ม กลัวเขาจะว่าป้าใจร้าย กว่าจะกล้าบอกว่าเจ็บ ป้าก็ทนไปตั้งนาน คราวหน้าลองเผื่อที่ให้ความโกรธบ้างนะ";
        if (weather_type==1)
            return "ต้นนี้ไปไม่รอดแล้วลูก ดินยังชื้นอยู่ลึกลงไป แต่รากมันไปไม่ถึง\n"
                   "ป้าเคยขายต้นไม้ดีอยู่หลายปี พอขายไม่ได้สักต้น วันนั้นถึงกับเก็บร้านกลับบ้านเลย กว่าจะกล้าเปิดอีกที ก็วันที่ป้ายอมบอกเพื่อนตรง ๆ ว่าป้ากลัว";
        if (r.missing[1] && r.missing[2])
            return "ต้นนี้ไปไม่รอดแล้วลูก พายุดึงรากจนดินเปิด เปลือกตรงกิ่งก็ปริ เห็นรอยตรงนี้ไหม\n"
                   "ปีแรกที่ป้าปลูก ทุกต้นโตดีหมด ป้าเลยนึกว่ารู้พอแล้ว พอเจอพายุทีเดียวถึงรู้ว่ายังมีอะไรให้หัดอีกเยอะ มาเถอะ ช่วยป้าดูรากต้นนี้กัน คราวหน้าเราจะได้รู้ว่าควรดูตรงไหน";
        return "ต้นนี้ไปไม่รอดแล้วลูก เมื่อคืนลมแรง รากยึดดินไว้ไม่อยู่จนต้นล้มทั้งต้น\n"
               "ตอนป้าปลูกต้นแรก ๆ ป้าดีใจทุกทีที่เห็นยอดสูงขึ้น จนวันที่มันล้มนั่นแหละ ป้าถึงได้ก้มดูรากเป็นครั้งแรก ล้มคราวนั้น ป้าก็ได้หัดปลูกใหม่อีกแบบ";
    }
    if (r.missing[2])
        return "ต้นนี้รอดมาได้นะ เปลือกตรงนี้ยังมีรอยอยู่เลย\n"
               "หนูเคยยิ้มทั้งที่ยังโกรธอยู่ไหม ป้าเคยนะ พอกล้าบอกเขาว่าคำไหนทำให้เราเจ็บ เขาถึงรู้ว่าคราวหน้าควรระวังตรงไหน";
    if (r.missing[1])
        return "ต้นนี้รอดมาได้นะ แต่รากยังสั้นอยู่ คราวหน้าลองเผื่อผลึกให้รากอีกหน่อย\n"
               "เวลาป้ากลัว ป้าชอบหาคนมานั่งคุยด้วย บางทีเขายังไม่ทันแนะนำอะไร พอได้เล่า ป้าก็รู้แล้วว่าตัวเองติดอยู่ตรงไหน";
    return "ต้นนี้โตได้ดีเลยลูก ผ่านเมื่อคืนมาได้ เช้านี้ยังไม่มีดอกก็จริง ลองดูตรงโคนสิ มันยังยืนอยู่ดี\n"
           "ป้าเคยใจร้อน อยากให้ต้นที่ปลูกออกดอกพร้อมข้างบ้าน เดี๋ยวนี้แค่เห็นมันผ่านคืนยาก ๆ มาได้ ป้าก็ดีใจเป็นแล้ว";

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
        if (stories[id].type!=type) continue;
        if (used>=day->gift[i]) { used-=day->gift[i]; continue; }
        printf("ผลึก%sจาก %s เมื่อวาน ",crystal[type],stories[id].name);
        if (move.b>=0) printf("ผสมเป็น “%s” ",recipe(move.a,move.b));
        printf("ได้เลี้ยง%sของต้นนี้\n",part[target(move)]);
        printf("“%s”\n",stories[id].nextMorning);
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
    printf("ป้าอิงก้มลงมองต้นไม้ “%s”\n",ending(p,r,day->weather));
    visitor_returns(day,p);
    printf("1. เปิดบันทึกการปลูก\n");
    if (!choose(1,1)) return;
    printf("สัดส่วนปุ๋ยที่ใส่ทั้งหมด\n");
    proportions(p);
    printf("อากาศเมื่อคืน: %s   ขาดราก %d หน่วย / เปลือก %d หน่วย   สุขภาพ %d/100\n",
           weather[day->weather],r.missing[1],r.missing[2],r.health);
    printf("เติบโต %.1f/30   สมดุล %.1f/40   หลังผ่านอากาศ %.1f/30\n",r.growth,r.balance,r.weatherScore);
    printf("คะแนนรวม %.0f/100%s\n",r.growth+r.balance+r.weatherScore,r.perfect?"   Perfect!":"");
}

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
        printf("1. เปิดประตูรับลูกค้า\n");
        if (!choose(1,1)) return 0;
        meet_customers(&day,&plan);
        if (inputEnded) return 0;
        planting(&day,&plan);
        if (inputEnded) return 0;
        grow_tree(&plan);
        if (inputEnded) return 0;
        morning(&day,&plan);
        if (inputEnded) return 0;
        printf("1. ดูสูตร Perfect ของวันนี้   2. เล่นรอบใหม่   3. ออกจากเกม\n");
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
