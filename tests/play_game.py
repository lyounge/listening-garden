# Development-only checks. The game itself needs only its single C file.
import re
import subprocess
import sys
import threading

exe = sys.argv[1]
def run(data):
    result = subprocess.run([exe], input=data.encode(), stdout=subprocess.PIPE, timeout=15)
    assert result.returncode == 0
    return result.stdout.decode("utf-8")

# Invalid numbers and truncated lines must never spill into another menu.
for bad in ("4294967297", "9" * 200, "1" + " " * 98 + "2", "abc", "-1", "1x"):
    text = run(bad + "\n")
    assert "08:00" not in text, bad
    assert "ได้รับผลึก:" not in text, bad
    text = run(bad + "\n1\n2\n3\n1\n1\n3\n")
    assert "คะแนนรวม" in text

# Full dialogue pauses, brief mode, and every help exit.
for prefix in ("1\n", "2\n0\n", "2\n1\n0\n",
               "2\n1\n1\n0\n", "2\n1\n1\n1\n0\n",
               "2\n1\n1\n1\n1\n1\n"):
    for mode in (1, 2):
        dialogue = "3\n\n" * 8 if mode == 1 else ""
        text = run(prefix + str(mode) + "\n" + dialogue + "3\n1\n1\n3\n")
        assert text.count("ได้รับผลึก:") == 8
        assert "คะแนนรวม 0/100" in text
        assert "ยังไม่ได้ใส่ปุ๋ย" in text

# Plan from the displayed inventory, without using the game's answer.
names = ["สุข", "ภูมิใจ", "เศร้า", "กลัว", "โกรธ", "ผิดหวัง"]
p = subprocess.Popen([exe], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
timer = threading.Timer(20, p.kill)
timer.start()
try:
    def read():
        data = b""
        while not data.endswith(b"> "):
            char = p.stdout.read(1)
            assert char, "Game ended before expected menu"
            data += char
        return data.decode("utf-8")
    def send(value):
        p.stdin.write((str(value) + "\n").encode())
        p.stdin.flush()
        return read()
    read()
    send(1)
    screen = send(2)
    bag = [int(re.search(r"(?<!\S)" + n + r" (\d+)", screen).group(1)) for n in names]
    goals = list(map(int, re.search(r"เป้าออกดอกคืนนี้: ใบ (\d+) / ราก (\d+) / เปลือก (\d+)", screen).groups()))
    values = [0, 0, 0]
    for group in range(3):
        while sum(bag[group*2:group*2+2]) + values[group]//10 < goals[group]//10:
            picks = [next(i for i in range(g*2, g*2+2) if bag[i])
                     for g in range(3) if g != group]
            screen = send(4)
            options = dict((name, num) for num, name in re.findall(
                r"(\d+)\. (สุข|ภูมิใจ|เศร้า|กลัว|โกรธ|ผิดหวัง) ×", screen))
            send(options[names[picks[0]]])
            send(options[names[picks[1]]])
            send(1)
            for i in picks:
                bag[i] -= 1
            values[group] += 10
    send(1)
    for group in range(3):
        while values[group] < goals[group]:
            screen = send(group+1)
            num, name, count = re.findall(
                r"(\d+)\. ใช้(สุข|ภูมิใจ|เศร้า|กลัว|โกรธ|ผิดหวัง) \(เหลือ (\d+) ชิ้น\)", screen)[0]
            amount = min(int(count), (goals[group]-values[group])//10)
            send(num)
            if int(count) > 1:
                send(amount)
            values[group] += amount*10
    send(0)
    send(3)
    tail, _ = p.communicate(b"1\n\n\n\n\n1\n3\n", timeout=10)
    text = tail.decode("utf-8")
    assert "คะแนนรวม 100/100" in text
    assert text.index("คะแนนรวม") < text.index("ลูกค้าแวะมาอีกครั้ง") < text.index("ป้าอิงฝากไว้")
    assert p.returncode == 0
finally:
    timer.cancel()
    if p.poll() is None:
        p.kill()
    p.wait()
print("PASS: input regressions, full/brief modes, help exits, empty ending and Perfect round")
