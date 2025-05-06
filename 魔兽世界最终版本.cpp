#include <iostream>
using namespace std;
#include <unordered_map>
#include <limits.h>
#include <vector>
#include <math.h>
#include <stdio.h>

vector<string> weapons = {"sword", "bomb", "arrow"};
unordered_map<string, int> warrior_hp;
unordered_map<string, int> warrior_sth;
int t;
int M, N, R, K, T;
int enemy_num1, enemy_num2;

class warrior;
class weapon
{
public:
    int type;       // 0sword,1bomb,2arrow
    int durability; // 还能使用的次数
    int strength;   // 攻击力
    bool usable;
    weapon(int t, int dur, int s) : type(t), durability(dur), strength(s), usable(true) {}
    virtual void attack(warrior *user, warrior *enemy) = 0;
    virtual ~weapon() {}
};
class sword : public weapon
{
public:
    sword(int s) : weapon(0, INT_MAX, s)
    {
        if (s == 0)
        {
            usable = false;
        }
    }
    void attack(warrior *user, warrior *enemy);
};
class bomb : public weapon
{
public:
    bomb() : weapon(1, 1, INT_MAX) {}
    void attack(warrior *user, warrior *enemy);
};
class arrow : public weapon
{
public:
    arrow() : weapon(2, 3, R) {}
    void attack(warrior *user, warrior *enemy);
};
class city
{
public:
    int flag;     // 旗子颜色。0没旗子，1红，2蓝
    int last_win; // 上一场战斗的胜者。 0平局，1红，2蓝
    int elements; // 城市内的生命元的数量
    // int index;
    vector<warrior *> warrior_in_city;
    city() : flag(0), last_win(0), elements(0), warrior_in_city(vector<warrior *>()) {}
    // city(const city &c) : flag(c.flag), last_win(c.last_win), elements(c.elements), warrior_in_city(vector<warrior *>(N + 2, nullptr)) {} // 我们只复制城市的几个数值信息
};
class warrior
{
public:
    string head_name;         // 司令部的名字(red or blue)
    int num;                  // 武士的编号
    string name;              // 武士的名字（种类）
    int hp;                   // 生命
    int force;                // 攻击力
    vector<weapon *> weapons; // 武器列表(由于只有狼会掠夺武器而且不会掠夺已有的武器，所以所有武士都不会有相同的武器，我们用一个长度为3的数组来存放武器)
    bool is_alive = true;     // 是否活着
    int pos;                  // 当前位置
    int init_pos;             // 初始位置
    bool reach_des = false;   // 是否到达了目的地(敌方司令部)
    double joy = 0.0;         // 士气值，只有dragon的士气是有用的
    int loyalty = 0;          // 忠诚度，只有lion的忠诚度是有用的

public:
    warrior(string hn, int n, string s) : head_name(hn), num(n), name(s), hp(warrior_hp[s]), force(warrior_sth[s]), weapons(vector<weapon *>(3, nullptr))
    {
        init_pos = pos = (head_name == "red") ? 0 : N + 1;
    }
    virtual ~warrior()
    {
        // for (int i = 0; i < 3; i++)
        // {
        //     delete weapons[i];
        // }
    }
    void march(int time, vector<city> &cities) // 前进
    {
        if (reach_des) // 到达目的地
        {
            cities[pos].warrior_in_city.push_back(this);
            return;
        }

        if (head_name == "red")
        {
            ++pos;
            // cities[pos].warrior_in_city.push_back((warrior *)this);
            cities[pos].warrior_in_city.push_back(this);
            if (pos == N + 1)
            {
                ++enemy_num2; // 蓝方司令部内的敌人加1
                // reach_des = true;
            }
        }
        else
        {
            --pos;
            cities[pos].warrior_in_city.push_back(this);
            if (pos == 0)
            {
                ++enemy_num1; // 红方司令部内敌人加1
                // reach_des = true;
            }
        }

        if (name == "iceman" && abs(pos - init_pos) % 2 == 0)
        {
            hp = max(1, hp - 9);
            force += 20;
        }
    }
    virtual void escape(int time) {}                      // 逃跑(只有lion需要重写)
    virtual void rob_weapons(int time, warrior *enemy) {} // 掠夺武器，只有wolf需要重写
    virtual void yell(int time) {}                        // 欢呼，只有dragon需要重写
    void report(int time)                                 // 报告
    {
        vector<int> rem;
        for (int i = 0; i < 3; ++i)
        {
            if (weapons[2 - i] && !weapons[2 - i]->usable)
            {
                delete weapons[2 - i];
                weapons[2 - i] = nullptr;
            }
            if (weapons[2 - i])
            {
                rem.push_back(2 - i);
            }
        }

        printf("%03d:55 %s %s %d has ", time, head_name.c_str(), name.c_str(), num);
        if (rem.size())
        {
            for (int j = 0; j < rem.size(); ++j)
            {
                if (j) // 不是第一个武器
                {
                    printf(",");
                }
                if (weapons[rem[j]]->type == 0) // sword
                {
                    printf("sword(%d)", weapons[rem[j]]->strength);
                }
                if (weapons[rem[j]]->type == 1)
                {
                    printf("bomb");
                }
                if (weapons[rem[j]]->type == 2)
                {
                    printf("arrow(%d)", weapons[rem[j]]->durability);
                }
            }
            printf("\n");
        }
        else
        {
            printf("no weapon\n");
        }
    }
    virtual void attack(warrior *enemy, int time, int city_index)
    {
        // 主动攻击
        int t1 = hp, t2 = enemy->hp;
        enemy->hp -= force;
        if (weapons[0] != nullptr)
        {
            weapons[0]->attack(this, enemy);
        }
        // 000:40 red iceman 1 attacked blue lion 1 in city 1 with 20 elements and force 30
        printf("%03d:40 %s %s %d attacked %s %s %d in city %d with %d elements and force %d\n", time, head_name.c_str(), name.c_str(), num,
               enemy->head_name.c_str(), enemy->name.c_str(), enemy->num, city_index, hp, force);
        if (enemy->hp <= 0)
        {
            enemy->is_alive = false;
            // 001:40 red lion 2 was killed in city 1
            printf("%03d:40 %s %s %d was killed in city %d\n", time, enemy->head_name.c_str(), enemy->name.c_str(), enemy->num, city_index);
            if (enemy->name == "lion") // lion战死后生命元给对方
            {
                hp += t2;
            }
        }

        if (enemy->is_alive && enemy->name != "ninja") // 反击
        {
            hp -= enemy->force / 2;
            if (enemy->weapons[0] != nullptr)
            {
                enemy->weapons[0]->attack(enemy, this);
            }
            // 001:40 blue dragon 2 fought back against red lion 2 in city 1
            printf("%03d:40 %s %s %d fought back against %s %s %d in city %d\n", time, enemy->head_name.c_str(), enemy->name.c_str(), enemy->num,
                   head_name.c_str(), name.c_str(), num, city_index);
        }

        if (hp <= 0)
        {
            is_alive = false;
            printf("%03d:40 %s %s %d was killed in city %d\n", time, head_name.c_str(), name.c_str(), num, city_index);
            if (name == "lion")
            {
                enemy->hp += t1;
            }
        }
    }
};
class dragon : public warrior
{
public:
    dragon(string hn, int n, double j) : warrior(hn, n, "dragon")
    {
        joy = j;
        switch (num % 3)
        {
        case 0:
            weapons[0] = new sword(int(0.2 * warrior_sth["dragon"]));
            break;
        case 1:
            weapons[1] = new bomb();
            break;
        case 2:
            weapons[2] = new arrow();
            break;
        default:
            break;
        }
    }
    void yell(int time) override
    {
        // 003:40 blue dragon 2 yelled in city 4
        if (joy > 0.8)
            printf("%03d:40 %s dragon %d yelled in city %d\n", time, head_name.c_str(), num, pos);
    }
};
class ninja : public warrior
{
public:
    ninja(string hn, int n) : warrior(hn, n, "ninja")
    {
        switch (num % 3)
        {
        case 0:
            weapons[0] = new sword(int(0.2 * warrior_sth["ninja"]));
            weapons[1] = new bomb();
            break;
        case 1:
            weapons[1] = new bomb();
            weapons[2] = new arrow();
            break;
        case 2:
            weapons[2] = new arrow();
            weapons[0] = new sword(int(0.2 * warrior_sth["ninja"]));
            break;
        default:
            break;
        }
    }
};
class iceman : public warrior
{
public:
    iceman(string hn, int n) : warrior(hn, n, "iceman")
    {
        switch (num % 3)
        {
        case 0:
            weapons[0] = new sword(int(0.2 * warrior_sth["iceman"]));
            break;
        case 1:
            weapons[1] = new bomb();
            break;
        case 2:
            weapons[2] = new arrow();
            break;
        default:
            break;
        }
    }
};
class lion : public warrior
{
public:
    lion(string hn, int n, int loy) : warrior(hn, n, "lion")
    {
        loyalty = loy;
    }
    void escape(int time) override
    {
        if (reach_des)
        {
            return;
        }

        if (loyalty <= 0)
        {
            printf("%03d:05 %s lion %d ran away\n", time, head_name.c_str(), num);
            is_alive = false; // 为了方便将逃跑的看作死掉的
        }
    }
};
class wolf : public warrior
{
public:
    wolf(string hn, int n) : warrior(hn, n, "wolf") {}
    void rob_weapons(int time, warrior *enemy) override
    {
        // 先把自己不能使用的武器清理掉
        for (int i = 0; i < 3; ++i)
        {
            if (weapons[i] && !weapons[i]->usable) // 武器不能用了就置为空指针
            {
                delete weapons[i];
                weapons[i] = nullptr;
            }
        }

        // 再收缴对方的能用的
        for (int i = 0; i < 3; ++i)
        {
            if (weapons[i] == nullptr && enemy->weapons[i] && enemy->weapons[i]->usable)
            {
                weapons[i] = enemy->weapons[i];
            }
        }
    }
};
class headquarter
{
public:
    string name;                  // 司令部的名称
    int hp;                       // 生命元
    int num;                      // 下一个需要制造的角色的编号
    int index;                    // 下一个制造武士的类型
    vector<string> warrior_order; // 制造武士的顺序

public:
    headquarter(string n, int h, const vector<string> &order) : name(n), hp(h), num(1), index(0), warrior_order(order) {}
    void produce_warriors(int time, vector<city> &cities)
    {
        string warrior_type = warrior_order[index];
        if (hp >= warrior_hp[warrior_type])
        {
            printf("%03d:00 %s %s %d born\n", time, name.c_str(), warrior_type.c_str(), num);
            hp -= warrior_hp[warrior_type];
            index = (index + 1) % 5;
            warrior *tmp; // 新生产的武士
            if (warrior_type == "dragon")
            {
                tmp = new dragon(name, num, (double)hp / warrior_hp["dragon"]);
            }
            else if (warrior_type == "ninja")
            {
                tmp = new ninja(name, num);
            }
            else if (warrior_type == "iceman")
            {
                tmp = new iceman(name, num);
            }
            else if (warrior_type == "lion")
            {
                tmp = new lion(name, num, hp);
            }
            else if (warrior_type == "wolf")
            {
                tmp = new wolf(name, num);
            }

            if (warrior_type == "lion")
            {
                printf("Its loyalty is %d\n", hp);
            }
            else if (warrior_type == "dragon")
            {
                printf("Its morale is %.2f\n", (double)hp / warrior_hp["dragon"]);
            }

            // 总部增加武士
            if (this->name == "red")
            {
                cities[0].warrior_in_city.push_back(tmp);
            }
            else
            {
                cities[N + 1].warrior_in_city.push_back(tmp);
            }
            ++num;
        }
    }
    void report(int time)
    {
        // 000:50 100 elements in red headquarter
        printf("%03d:50 %d elements in %s headquarter\n", time, hp, name.c_str());
    }
};
void sword::attack(warrior *user, warrior *enemy)
{
    // printf("use a sword***********\n");
    enemy->hp -= strength;
    if (enemy->hp <= 0)
    {
        enemy->is_alive = false;
    }
    strength = 0.8 * strength; // sword使用之后会变钝
    if (strength == 0)
    {
        usable = false;
    }
}
void bomb::attack(warrior *user, warrior *enemy)
{
    // printf("use a bomb *************\n");
    user->is_alive = enemy->is_alive = false; // 同归于尽
    --durability;
    usable = false;
}
void arrow::attack(warrior *user, warrior *enemy)
{
    // printf("use a arrow******\n");
    enemy->hp -= R;
    if (--durability == 0)
    {
        usable = false;
    }
    if (enemy->hp <= 0)
    {
        enemy->is_alive = false;
    }
}

int main()
{
    freopen("case17.txt", "w", stdout);

    scanf("%d", &t);
    for (int i = 1; i <= t; ++i)
    {
        scanf("%d %d %d %d %d", &M, &N, &R, &K, &T);
        scanf("%d %d %d %d %d", &warrior_hp["dragon"], &warrior_hp["ninja"], &warrior_hp["iceman"], &warrior_hp["lion"], &warrior_hp["wolf"]);      // 生命值
        scanf("%d %d %d %d %d", &warrior_sth["dragon"], &warrior_sth["ninja"], &warrior_sth["iceman"], &warrior_sth["lion"], &warrior_sth["wolf"]); // 攻击力
        printf("Case %d:\n", i);

        int hours = 0, minutes = 0; // 前者是小时数，后者是总时间数
        enemy_num1 = enemy_num2 = 0;
        headquarter red("red", M, {"iceman", "lion", "wolf", "ninja", "dragon"});
        headquarter blue("blue", M, {"lion", "dragon", "ninja", "iceman", "wolf"});
        vector<city> cities(N + 2); // 每个内层vector存放处在对应编号城市的武士

        while (minutes <= T)
        {
            vector<city> new_cities(N + 2); // 记录武士前进以及战斗之后城市内武士的情况
            for (int i = 0; i < N + 2; i++)
            {
                new_cities[i].elements = cities[i].elements;
                new_cities[i].flag = cities[i].flag;
                new_cities[i].last_win = cities[i].last_win;
            }

            // 00分生产武士
            red.produce_warriors(hours, cities);
            blue.produce_warriors(hours, cities);

            // 05分lion逃跑
            minutes += 5;
            if (minutes > T)
            {
                break;
            }
            for (auto &city : cities)
            {
                for (warrior *war : city.warrior_in_city)
                {
                    if (war->is_alive)
                        war->escape(hours);
                }
            }

            // 10分所有活着的武士前进
            minutes += 5;
            if (minutes > T)
            {
                break;
            }
            for (auto &city : cities)
            {
                for (warrior *war : city.warrior_in_city)
                {
                    if (war->is_alive)
                    {
                        war->march(hours, new_cities);
                    }
                }
            }
            for (int i = 0; i < N + 2; ++i) // 由于事件要求从西到东报告，所以必须先移动之后再从西向东遍历城市内的武士(故输出内容没有放在前进的函数中)
            {
                auto &city = new_cities[i];
                if (i == 0) // 红司令部
                {
                    for (warrior *war : city.warrior_in_city)
                    {
                        if (war->head_name == "blue" && !war->reach_des)
                        {
                            printf("%03d:10 blue %s %d reached red headquarter with %d elements and force %d\n", hours, war->name.c_str(), war->num, war->hp, war->force);
                            if (enemy_num1 == 2)
                            {
                                printf("%03d:10 red headquarter was taken\n", hours);
                            }
                            war->reach_des = true; // 到达司令部的报告只会输出一次
                        }
                    }
                }
                else if (i == N + 1) // 蓝司令部
                {
                    for (warrior *war : city.warrior_in_city)
                    {
                        if (war->head_name == "red" && !war->reach_des)
                        {
                            printf("%03d:10 red %s %d reached blue headquarter with %d elements and force %d\n", hours, war->name.c_str(), war->num, war->hp, war->force);
                            if (enemy_num2 == 2)
                            {
                                printf("%03d:10 blue headquarter was taken\n", hours);
                            }
                            war->reach_des = true;
                        }
                    }
                }
                else
                {
                    // 城市内如果既有红也有蓝，那么红武士是从西边过来的，它在city中的顺序一定在蓝武士之前
                    // 并且new_cities中城市内的武士都是存活的(march中把战死的武士去掉了)
                    for (warrior *war : city.warrior_in_city)
                    {
                        if (war->is_alive)
                        {
                            printf("%03d:10 %s %s %d marched to city %d with %d elements and force %d\n", hours, war->head_name.c_str(), war->name.c_str(),
                                   war->num, war->pos, war->hp, war->force);
                        }
                    }
                }
            }

            if (enemy_num1 == 2 || enemy_num2 == 2) // 有一方的司令部被占领，游戏结束
            {
                break;
            }

            // 20分每个城市产生10个生命元
            minutes += 10;
            if (minutes > T)
            {
                break;
            }
            for (int i = 1; i <= N; ++i)
            {
                // new_cities[i].elements = cities[i].elements + 10;
                new_cities[i].elements += 10;
            }

            // 30分如果只有一个武士就直接获取城市内的生命元
            minutes += 10;
            if (minutes > T)
            {
                break;
            }
            for (int i = 1; i <= N; ++i)
            {
                if (new_cities[i].warrior_in_city.size() == 1)
                {
                    // 取走所有生命元
                    warrior *war = new_cities[i].warrior_in_city[0];
                    if (war->head_name == "red")
                    {
                        red.hp += new_cities[i].elements;
                        printf("%03d:30 red %s %d earned %d elements for his headquarter\n", hours, war->name.c_str(),
                               war->num, new_cities[i].elements);
                    }
                    else
                    {
                        blue.hp += new_cities[i].elements;
                        printf("%03d:30 blue %s %d earned %d elements for his headquarter\n", hours, war->name.c_str(),
                               war->num, new_cities[i].elements);
                    }
                    new_cities[i].elements = 0;
                }
            }

            // 35分有arrow的武士放箭
            minutes += 5;
            if (minutes > T)
            {
                break;
            }
            for (int i = 0; i < N + 2; ++i)
            {
                city &this_city = new_cities[i];
                for (warrior *war : this_city.warrior_in_city)
                {
                    if (war->weapons[2] == nullptr || war->reach_des) // 没有arrow或已经到达目的地的直接跳过
                    {
                        continue;
                    }

                    // 检查下一座城市中有无敌人
                    warrior *enemy = nullptr;
                    city &next_city = war->head_name == "red" ? new_cities[i + 1] : new_cities[i - 1];
                    for (warrior *w : next_city.warrior_in_city)
                    {
                        if (w->head_name != war->head_name)
                        {
                            enemy = w;
                            break;
                        }
                    }
                    if (enemy) // 有敌人，放箭
                    {
                        war->weapons[2]->attack(war, enemy);
                        if (!enemy->is_alive) // 射杀敌人
                        {
                            // 000:35 blue dragon 1 shot and killed red lion 4
                            printf("%03d:35 %s %s %d shot and killed %s %s %d\n", hours, war->head_name.c_str(), war->name.c_str(), war->num,
                                   enemy->head_name.c_str(), enemy->name.c_str(), enemy->num);
                        }
                        else
                        {
                            // 000:35 blue dragon 1 shot
                            printf("%03d:35 %s %s %d shot\n", hours, war->head_name.c_str(), war->name.c_str(), war->num);
                        }
                    }
                }
            }

            // 38分使用bomb(司令部内的武士不会使用bomb)
            minutes += 3;
            if (minutes > T)
            {
                break;
            }
            for (int i = 1; i <= N; ++i)
            {
                city &this_city = new_cities[i];
                if (this_city.warrior_in_city.size() < 2)
                {
                    continue;
                }
                warrior *war_r = this_city.warrior_in_city[0], *war_b = this_city.warrior_in_city[1];
                if (war_r->is_alive && war_b->is_alive) // 双方都未被arrow射杀
                {
                    if (this_city.flag == 1 || (this_city.flag == 0 && i % 2 == 1)) // 红方先进攻
                    {
                        if ((war_r->weapons[0] != nullptr && war_r->force + war_r->weapons[0]->strength < war_b->hp) ||
                            (war_r->weapons[0] == nullptr && war_r->force < war_b->hp)) // 无法杀死对方
                        {
                            if (war_b->name != "ninja" && (war_b->weapons[0] != nullptr && war_b->force / 2 + war_b->weapons[0]->strength >= war_r->hp) ||
                                (war_b->weapons[0] == nullptr && war_b->force / 2 >= war_r->hp)) // 会被对方反击杀死
                            {
                                if (war_r->weapons[1] != nullptr)
                                {
                                    war_r->weapons[1]->attack(war_r, war_b);
                                    // 000:38 blue dragon 1 used a bomb and killed red lion 7
                                    printf("%03d:38 red %s %d used a bomb and killed blue %s %d\n", hours, war_r->name.c_str(), war_r->num, war_b->name.c_str(), war_b->num);
                                    continue;
                                }
                            }
                        }
                        else if ((war_r->weapons[0] != nullptr && war_r->force + war_r->weapons[0]->strength >= war_b->hp) ||
                                 (war_r->weapons[0] == nullptr && war_r->force >= war_b->hp))
                        {
                            if (war_b->weapons[1] != nullptr)
                            {
                                war_b->weapons[1]->attack(war_b, war_r);
                                printf("%03d:38 blue %s %d used a bomb and killed red %s %d\n", hours, war_b->name.c_str(), war_b->num, war_r->name.c_str(), war_r->num);
                                continue;
                            }
                        }
                    }
                    else // 蓝方先进攻
                    {
                        if ((war_b->weapons[0] != nullptr && war_b->force + war_b->weapons[0]->strength < war_r->hp) ||
                            (war_b->weapons[0] == nullptr && war_b->force < war_r->hp)) // 无法杀死对方
                        {
                            if (war_r->name != "ninja" && (war_r->weapons[0] != nullptr && war_r->force / 2 + war_r->weapons[0]->strength >= war_b->hp) ||
                                (war_r->weapons[0] == nullptr && war_r->force / 2 >= war_b->hp)) // 会被对方反击杀死
                            {
                                if (war_b->weapons[1] != nullptr)
                                {
                                    war_b->weapons[1]->attack(war_b, war_r);
                                    printf("%03d:38 blue %s %d used a bomb and killed red %s %d\n", hours, war_b->name.c_str(), war_b->num, war_r->name.c_str(), war_r->num);
                                    continue;
                                }
                            }
                        }
                        else if ((war_b->weapons[0] != nullptr && war_b->force + war_b->weapons[0]->strength >= war_r->hp) ||
                                 (war_b->weapons[0] == nullptr && war_b->force >= war_r->hp))
                        {
                            // printf("*****\n");
                            if (war_r->weapons[1] != nullptr)
                            {
                                // printf("????\n");
                                war_r->weapons[1]->attack(war_r, war_b);
                                printf("%03d:38 red %s %d used a bomb and killed blue %s %d\n", hours, war_r->name.c_str(), war_r->num, war_b->name.c_str(), war_b->num);
                                // cout << war_r->weapons[1]->type << " " << war_r->weapons[1]->durability << " " << war_r->weapons[1]->usable << endl;
                                // cout << war_r->is_alive << " " << war_b->is_alive << endl;
                                continue;
                            }
                        }
                    }
                }
            }

            // 40分发生战斗(只有城市内有2个武士的时候才会发生)
            // 战斗结束之后依次奖励生命元，并回收城市内的生命元
            minutes += 2;
            if (minutes > T)
            {
                break;
            }
            vector<int> winners(N + 2, 0); // 标记每座城市中的胜利方，如果没有发生战争或者平局(即没有人可以回收生命元)，就标记为0
            for (int i = 1; i <= N; ++i)
            {
                city &this_city = new_cities[i];
                if (this_city.warrior_in_city.size() < 2) // 城市只有一个人，应该在30分回收过生命元
                {
                    continue;
                }

                warrior *war_r = this_city.warrior_in_city[0], *war_b = this_city.warrior_in_city[1];
                if (!war_r->is_alive && !war_b->is_alive) // 都被射死了或者炸死了,没有发生战斗
                {
                    continue;
                }
                if (war_r->is_alive && war_b->is_alive) // 都活着，需要进行战斗
                {
                    if (this_city.flag == 1 || (this_city.flag == 0 && i % 2 == 1)) // 红方先手
                    {
                        war_r->attack(war_b, hours, i);
                    }
                    else // 蓝方先手
                    {
                        war_b->attack(war_r, hours, i);
                    }
                }

                // 战斗结束(至少一人存活)
                // 士气和忠诚度变化并欢呼
                if (war_r->is_alive)
                {
                    if (war_b->is_alive)
                    {
                        war_r->joy -= 0.2;
                        war_r->loyalty -= K;
                    }
                    else
                    {
                        war_r->joy += 0.2;
                    }

                    if (this_city.flag == 1 || (this_city.flag == 0 && i % 2 == 1))
                    {
                        war_r->yell(hours);
                    }
                }
                if (war_b->is_alive)
                {
                    if (war_r->is_alive)
                    {
                        war_b->joy -= 0.2;
                        war_b->loyalty -= K;
                    }
                    else
                    {
                        war_b->joy += 0.2;
                    }

                    if (this_city.flag == 2 || (this_city.flag == 0 && i % 2 == 0))
                    {
                        war_b->yell(hours);
                    }
                }

                if (war_r->is_alive && !war_b->is_alive) // 红胜出
                {
                    war_r->rob_weapons(hours, war_b);
                    winners[i] = 1;

                    printf("%03d:40 red %s %d earned %d elements for his headquarter\n", hours, war_r->name.c_str(),
                           war_r->num, this_city.elements);

                    // 插旗子
                    if (this_city.last_win == 1 && this_city.flag != 1)
                    {
                        // 004:40 blue flag raised in city 4
                        printf("%03d:40 red flag raised in city %d\n", hours, i);
                        this_city.flag = 1;
                    }
                    this_city.last_win = 1;
                }
                else if (!war_r->is_alive && war_b->is_alive) // 蓝胜出
                {
                    war_b->rob_weapons(hours, war_r);
                    winners[i] = 2;

                    printf("%03d:40 blue %s %d earned %d elements for his headquarter\n", hours, war_b->name.c_str(),
                           war_b->num, this_city.elements);

                    // 插旗子
                    if (this_city.last_win == 2 && this_city.flag != 2)
                    {
                        printf("%03d:40 blue flag raised in city %d\n", hours, i);
                        this_city.flag = 2;
                    }
                    this_city.last_win = 2;
                }
                else // 平局
                {
                    this_city.last_win = 0;
                }
            }
            for (int i = N; i >= 1; --i) // 红方奖励生命元
            {
                if (winners[i] == 1 && red.hp >= 8)
                {
                    new_cities[i].warrior_in_city[0]->hp += 8;
                    red.hp -= 8;
                }
            }
            for (int i = 1; i <= N; ++i) // 蓝方奖励生命元
            {
                if (winners[i] == 2 && blue.hp >= 8)
                {
                    new_cities[i].warrior_in_city[1]->hp += 8;
                    blue.hp -= 8;
                }
            }
            // 回收生命元
            for (int i = 1; i <= N; i++)
            {
                city &this_city = new_cities[i];
                if (this_city.warrior_in_city.size() < 2) // 未发生过战斗
                {
                    continue;
                }

                warrior *war_r = this_city.warrior_in_city[0], *war_b = this_city.warrior_in_city[1];
                if (winners[i] == 1) // 红方回收
                {
                    red.hp += this_city.elements;
                    this_city.elements = 0;
                }
                else if (winners[i] == 2) // 蓝方回收
                {
                    blue.hp += new_cities[i].elements;
                    this_city.elements = 0;
                }
            }

            // 50分司令部报告拥有的生命元数量
            minutes += 10;
            if (minutes > T)
            {
                break;
            }
            red.report(hours);
            blue.report(hours);

            // 55分武士报告武器的情况
            minutes += 5;
            if (minutes > T)
            {
                break;
            }
            for (int i = 0; i < N + 2; ++i) // 红武士汇报
            {
                city &this_city = new_cities[i];
                for (warrior *war : this_city.warrior_in_city)
                {
                    if (war->is_alive && war->head_name == "red")
                    {
                        war->report(hours);
                    }
                }
            }
            for (int i = 0; i < N + 2; ++i) // 蓝武士汇报
            {
                city &this_city = new_cities[i];
                for (warrior *war : this_city.warrior_in_city)
                {
                    if (war->is_alive && war->head_name == "blue")
                    {
                        war->report(hours);
                    }
                }
            }

            minutes += 5;
            ++hours;
            cities = new_cities;
        }
    }
}
