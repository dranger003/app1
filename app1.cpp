#define RED     0 // 155 - 2.0V
#define GREEN   1 // 247 - 3.2V
#define BLUE    2 // 240 - 3.1V
#define SWITCH  3
#define RELAY   4

typedef struct
{
    int p;
    int v;
    int s;
} pv_t;

typedef struct
{
    pv_t pvs[5];
    int v;
} ctrl_t;

ctrl_t ctrls[] =
{
    {
        {
            { A0, 155, 0 }, // R
            { A1, 247, 0 }, // G
            { A4, 240, 0 }, // B
            { D0, LOW, 0 }, // S
            { D1, LOW, 0 }, // R
        },
        LOW,
    },
    {
        {
            { A5, 155, 0 }, // R
            { A6, 247, 0 }, // G
            { A7, 240, 0 }, // B
            { D2, LOW, 0 }, // S
            { D3, LOW, 0 }, // R
        },
        LOW,
    },
};

int v = 0, d = 1;
unsigned long t = millis();

void reset()
{
    for (int c = 0; c < sizeof(ctrls) / sizeof(ctrl_t); c++)
    {
        analogWrite(ctrls[c].pvs[BLUE].p, ctrls[c].pvs[BLUE].v);

        for (int pv = 0; pv < sizeof(ctrls[c].pvs) / sizeof(pv_t); pv++)
        {
            int p = ctrls[c].pvs[pv].p;

            switch (pv)
            {
            case RED:
            case GREEN:
            case BLUE:
            case RELAY:
                pinMode(p, OUTPUT);
                digitalWrite(p, LOW);
                break;
            case SWITCH:
                pinMode(p, INPUT_PULLUP);
                ctrls[c].pvs[pv].v = -1;
                break;
            default:
                break;
            }
        }
    }
}

void buttonDown(int c)
{
    ctrls[c].pvs[RELAY].v = !ctrls[c].pvs[RELAY].v;
    digitalWrite(ctrls[c].pvs[RELAY].p, ctrls[c].pvs[RELAY].v);

    ctrls[c].pvs[BLUE].s = 1;
    analogWrite(ctrls[c].pvs[BLUE].p, 0);

    analogWrite(ctrls[c].pvs[ctrls[c].pvs[RELAY].v].p, ctrls[c].pvs[ctrls[c].pvs[RELAY].v].v);
}

void buttonUp(int c)
{
    analogWrite(ctrls[c].pvs[ctrls[c].pvs[RELAY].v].p, 0);

    analogWrite(ctrls[c].pvs[BLUE].p, v);
    ctrls[c].pvs[BLUE].s = 0;
}

void breathe(int c)
{
    if (ctrls[c].pvs[BLUE].s)
        return;

    int nt = millis();
    if (t != nt)
    {
        t = nt;

        !d ? --v : ++v;
        if (v < 0 || v > ctrls[c].pvs[BLUE].v)
        {
            if (v < 0)
                v = 0;
            else if (v > ctrls[c].pvs[BLUE].v)
                v = ctrls[c].pvs[BLUE].v;

            d = !d;
        }

        analogWrite(ctrls[c].pvs[BLUE].p, v);
    }
}

void setup()
{
    reset();
}

void loop()
{
    for (int c = 0; c < sizeof(ctrls) / sizeof(ctrl_t); c++)
    {
        breathe(c);

        int v = digitalRead(ctrls[c].pvs[SWITCH].p);
        if (v != ctrls[c].v)
        {
            v == LOW ? buttonDown(c) : buttonUp(c);
            ctrls[c].v = v;
        }
    }
}
