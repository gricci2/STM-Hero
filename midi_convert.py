import mido
from collections import defaultdict

MIDI_FILE = "Tetris.mid"
MAX_TIME_SEC = 180  # 3 minutes
BUCKET_SIZE = 0.05  # 50ms chord grouping window

mid = mido.MidiFile(MIDI_FILE)

ticks_per_beat = mid.ticks_per_beat
tempo = 500000

current_time = 0.0

buckets = defaultdict(list)
output_notes = []





all_notes = []

for track in mid.tracks:
    for msg in track:
        if msg.type == "note_on" and msg.velocity > 0:
            all_notes.append(msg.note)

LOW_NOTE = min(all_notes)
HIGH_NOTE = max(all_notes)

print(LOW_NOTE, HIGH_NOTE)

NUM_LANES = 8

def note_to_lane(note):
    span = HIGH_NOTE - LOW_NOTE + 1
    lane_size = span / NUM_LANES

    lane = int((note - LOW_NOTE) / lane_size)

    return min(lane, NUM_LANES - 1)

for msg in mido.merge_tracks(mid.tracks):

   
    if msg.time > 0:
        current_time += mido.tick2second(msg.time, ticks_per_beat, tempo)

    
    if msg.type == "set_tempo":
        tempo = msg.tempo

    
    if current_time > MAX_TIME_SEC:
        break

    
    if msg.type == "note_on" and msg.velocity > 0:

        bucket_time = int(current_time / BUCKET_SIZE) * BUCKET_SIZE

        buckets[bucket_time].append({
            "note": msg.note,
            "lane": note_to_lane(msg.note),
            "vel": msg.velocity
        })


for t in sorted(buckets.keys()):

    window = buckets[t]
    if not window:
        continue

    
    max_vel = max(n["vel"] for n in window)

    kept = [
        n for n in window
        if n["vel"] >= max_vel * 0.85
    ]


    lanes_bitmask = 0

    for n in kept:
        lanes_bitmask |= (1 << n["lane"])

    output_notes.append({
        "time": int(t * 1000),
        "lanes": lanes_bitmask
    })


print("typedef struct { uint16_t time; uint8_t lanes; } Note;\n")
print("Note song[] = {")

for n in output_notes:
    print(f"    {{{n['time'] *1000 + 5000000}, 0x{n['lanes']:02X}}},")

print("};")

print(f"\n// Total events: {len(output_notes)}")