#define RED     0 // 155 - 2.0V
#define GREEN   1 // 247 - 3.2V
#define BLUE    2 // 240 - 3.1V
#define SWITCH  3
#define RELAY   4

typedef struct
{
    int p;
    int v;
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
            { A0, 155 }, // R
            { A1, 247 }, // G
            { A4, 120 }, // B
            { D0, LOW }, // S
            { D1, LOW }, // R
        },
        LOW,
    },
    {
        {
            { A5, 155 }, // R
            { A6, 247 }, // G
            { A7, 120 }, // B
            { D2, LOW }, // S
            { D3, LOW }, // R
        },
        LOW,
    },
};

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

void setup()
{
    reset();
}

void buttonDown(int c)
{
    ctrls[c].pvs[RELAY].v = !ctrls[c].pvs[RELAY].v;
    digitalWrite(ctrls[c].pvs[RELAY].p, ctrls[c].pvs[RELAY].v);

	analogWrite(ctrls[c].pvs[BLUE].p, 0);
    analogWrite(ctrls[c].pvs[ctrls[c].pvs[RELAY].v].p, ctrls[c].pvs[ctrls[c].pvs[RELAY].v].v);
}

void buttonUp(int c)
{
    analogWrite(ctrls[c].pvs[ctrls[c].pvs[RELAY].v].p, 0);
	analogWrite(ctrls[c].pvs[BLUE].p, ctrls[c].pvs[BLUE].v);
}

void loop()
{
    for (int c = 0; c < sizeof(ctrls) / sizeof(ctrl_t); c++)
    {
        int v = digitalRead(ctrls[c].pvs[SWITCH].p);
        if (v != ctrls[c].v)
        {
            v == LOW ? buttonDown(c) : buttonUp(c);
            ctrls[c].v = v;
        }
    }
}
