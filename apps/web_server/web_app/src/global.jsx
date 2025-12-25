

export default function firmwareVersionToString(v) {
    const x = (v >> 16) & 0xff;
    const y = (v >> 8) & 0xff;
    const z = v & 0xff;
    return `${x}.${y}.${z}`;
}