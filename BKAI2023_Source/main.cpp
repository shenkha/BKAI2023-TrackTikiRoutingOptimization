
#include<bits/stdc++.h>
using namespace std;
struct Time {
    int hh, mm, ss;
    int secCount;
    Time(int hh, int mm, int ss): hh(hh), mm(mm), ss(ss) {
        secCount = hh * 3600 + mm * 60 + ss;
    }

    Time (int secCount) {
        this->secCount = secCount;
        ss = secCount % 60;
        secCount /= 60;
        mm = secCount % 60;
        secCount /= 60;
        hh = secCount;
    }
    Time() {
        *(this) = Time(0, 0, 0);
    }
    Time(string s) {
        // s must be hh:mm:ss
        hh = (s[0] - '0') * 10 + (s[1] - '0');
        mm = (s[3] - '0') * 10 + (s[4] - '0');
        ss = (s[6] - '0') * 10 + (s[7] - '0');
        secCount = hh * 3600 + mm * 60 + ss;
    }
    void printTime(string s) {
        cout << setw(2) << setfill('0') << hh << ":";
        cout << setw(2) << setfill('0') << mm << ":";
        cout << setw(2) << setfill('0') << ss;
        // cout << secCount;
        cout << s;
    }
    bool operator < (Time other) {
        return secCount < other.secCount;
    }

    bool operator == (Time other) {
        return secCount == other.secCount;
    }
};
struct TimeLine {
    Time start_, end_;
    TimeLine() {
        start_ = Time(0, 0, 0);
        end_ = Time(0, 0, 0);
    }
    TimeLine(Time start_, Time end_) : start_(start_), end_(end_) {};
    void readTimeLine() {
        string sstart, send;
        cin >> sstart >> send;
        start_ = Time(sstart);
        end_ = Time(send);
    }
    bool operator < (const TimeLine &other) {
        return 0;
        // return (start_ == other.start_) ? 
        //     end_ < other.end_ :
        //     start_ < other.start_;
    }
};
class Order {
public:
    /*
        - id        : id cua yeu cau van chuyen
        - hubStart  : ID cua Hub nhan (noi hang duoc lay)
        - hubEnd    : ID cua Hub tra (noi hang duoc tra)
        - weight    : khoi luong cua goi hang (kg)
        - volume    : the tich cua goi hang (m3)
        - timeP     : thoi gian boc hang len xe (giay)
        - timeD     : thoi gian do hang xuong xe (giay)
        - periodP   : khoang thoi gian boc hang hop le (hh:mm:ss -> hh:mm:ss)
        - periodD   : khoang thoi gian do hang hop le (hh:mm:ss -> hh:mm:ss)
    */
    int id;
    int hubStart, hubEnd;
    double weight, volume;
    int timeP, timeD;
    TimeLine periodP, periodD;
    Order() {};
    void readOrder(int id) {
        this->id = id;
        cin >> hubStart >> hubEnd;
        cin >> weight >> volume;
        cin >> timeP >> timeD;
        periodP.readTimeLine();
        periodD.readTimeLine();
    }
};
class Truck {
public:
    /*
        - id        : id cua xe tai
        - idHub     : id Hub xe tai bat dau va ket thuc
        - timeValid : khoang thoi gian xe tai lam viec (hh:mm:ss -> hh:mm:ss)
        - maxWeight : tai trong toi da cua xe tai (kg)
        - maxVolume : the tich toi da cua xe tai (m3)
        - velocity  : van toc cua xe tai (m/s)
    */
    int id;
    int idHub;
    TimeLine timeValid;
    double maxWeight, maxVolume;
    double velocity;
    Truck() {};
    void readTruck(int id) {
        this->id = id;
        cin >> idHub;
        timeValid.readTimeLine();
        cin >> maxWeight;
        cin >> maxVolume;
        cin >> velocity;
        velocity /= 3.6;
    }
};
class Instance {
public:
    /*
        nHub:   so luong hub
        nTruck: so luong xe tai
        nOrder: so luong don hang
        dist:   ma tran khoang cach
        truckes:    thong tin cac xe tai
        orders: thong tin cac order
        orderTakeAtHub: chi so cac order nhan tai hub
        orderReturnAtHub:   chi so cac order tra tai hub
    */
    int nHub, nTruck, nOrder;
    vector<vector<int>> dist;
    vector<Truck> truckes;
    vector<Order> orders;
    vector<vector<int>> orderTakeAtHub;
    vector<vector<int>> orderReturnAtHub;
    Instance() {};
    void readInstance() {
        cin >> nHub;
        orderTakeAtHub.resize(nHub + 1);
        orderReturnAtHub.resize(nHub + 1);
        dist.resize(nHub + 1, vector<int> (nHub + 1, 0));
        for (int i = 1; i <= nHub; i++)
            for (int j = 1; j <= nHub; j++) {
                cin >> dist[i][j];
                dist[i][j] *= 1000;
            }
        cin >> nTruck;
        truckes.resize(nTruck + 1);
        for (int i = 1; i <= nTruck; i++)
            truckes[i].readTruck(i);
            
        cin >> nOrder;
        orders.resize(nOrder + 1);
        for (int i = 1; i <= nOrder; i++) {
            orders[i].readOrder(i);
            orderTakeAtHub[orders[i].hubStart].push_back(i);
            orderReturnAtHub[orders[i].hubEnd].push_back(i);
        }
    }
};

class Picking {
typedef pair<bool, pair<Time, Time>> PICK;
#define isPick first
#define pickTime second.first
#define returnTime second.second
public:
    /*
        Picking the hien chien luoc cua mot xe tai 
        idTruck:    id cua xe tai thuc hien picking
        tour:   tour cua xe tai
        pick:   the hien minh se tuong tac voi nhung item nao
        + first:    true neu pick, false neu khong pick
        + second.first:   thoi diem pick
        + second.second:    thoi diem tra hang
        visitTimeLine:  thoi gian xe tai o lai tai tung hub tuong ung voi tour
    */
    int idTruck;
    double remainWeight, remainVolume;
    vector<int> tour;
    vector<PICK> pick;
    vector<TimeLine> visitTimeLine;
    vector<vector<int>> orderTakeAtHub;
    vector<vector<int>> orderReturnAtHub;
    void addItem(int i, Instance &instance) {
        pick[i].first = true;
        Order &order = instance.orders[i];
        orderTakeAtHub[order.hubStart].push_back(i);
        orderReturnAtHub[order.hubEnd].push_back(i);
    }
    void removeItem(int i, Instance &instance) {
        pick[i].first = false;
        Order &order = instance.orders[i];
        orderTakeAtHub[order.hubStart].pop_back();
        orderReturnAtHub[order.hubEnd].pop_back();
    }
    Picking() {}
};

#define iii pair<int, pair<int, int>>
class Solution {
typedef vector<int> TOUR;
public:
    Instance instance;
    vector<Picking> pickingPlan;
    vector<int> orderState;
    Solution(Instance instance) {
        this->instance = instance;
        pickingPlan.resize(instance.nTruck + 1);
        for (int i = 1; i <= instance.nTruck; i++) {
            Truck &truck = instance.truckes[i];
            pickingPlan[i].tour.push_back(truck.idHub);
            pickingPlan[i].pick.resize(instance.nOrder + 1, {0, {Time(),Time()}});
            pickingPlan[i].orderReturnAtHub.resize(instance.nHub + 1);
            pickingPlan[i].orderTakeAtHub.resize(instance.nHub + 1);
            pickingPlan[i].remainVolume = truck.maxVolume;
            pickingPlan[i].remainWeight = truck.maxWeight;
        }
        orderState.resize(instance.nOrder + 1, 0);
    }
    int objective;
    
    int inTour(int u, int truckId) {
        int pos = 0;
        for (auto v:pickingPlan[truckId].tour) {
            if (u == v) return pos;
            ++pos;            
        }
        return __INT_MAX__;
    }

    int evaluate(int truckId, int printAns = 0) {
        Truck &truck = instance.truckes[truckId];
        vector<int> &tour = pickingPlan[truckId].tour;
        int startTime = truck.timeValid.start_.secCount;
        int endTime = truck.timeValid.end_.secCount;
        double velocity = truck.velocity;
        
        int curTime = startTime;
        int ret = -1;
        // thua toan tham lam kiem tra xem co xu li duoc tat ca don hang can xu li hay khong, 
        // PHAI XU LI TAT CA
        for (int i = 0; i < tour.size(); i++) {
            int arrivetime = curTime;
            int norder = 0;
            vector<pair<TimeLine, pair<int, int>>> requests;
            for (int id:pickingPlan[truckId].orderTakeAtHub[tour[i]]) {
                requests.push_back({instance.orders[id].periodP,{instance.orders[id].timeP, id}});
                ++norder;
            }
            for (int id:pickingPlan[truckId].orderReturnAtHub[tour[i]]) {
                requests.push_back({instance.orders[id].periodD,{instance.orders[id].timeD, id}});
                ++norder;
            }
            auto cmp = [&] (pair<TimeLine, pair<int, int>> &a,pair<TimeLine, pair<int, int>> &b) {
                return a.first < b.first;
            };
            // sort theo thoi gian bat dau
            sort(requests.begin(), requests.end(), cmp);
            priority_queue<iii, vector<iii>, greater<iii>> pq;
            // khong toi uu, nhung period la 30p thi tam chap nhan duoc
            vector<pair<int, int> > orderOfOrder;

            for (int j = 0; j < requests.size(); ) {
                // neu thoi gian hien tai nho hon request gan nhat thi phai wait
                if (curTime < requests[j].first.start_.secCount)
                    curTime = requests[j].first.start_.secCount;
                // push tat ca request co the duoc xu li ngay lap tuc
                while (j < requests.size() && requests[j].first.start_.secCount <= curTime) {
                    pq.push({requests[j].first.end_.secCount, requests[j].second});
                    ++j;
                }
                // thoi gian gan nhat co request tiep theo
                int nextTime = (j >= requests.size()) ? __INT_MAX__:requests[j].first.start_.secCount;
                // xu li den khi co the xu li duoc request tiep theo
                while (curTime < nextTime && pq.size()) {
                    // uu tien xu li request co deadline som hon
                    int deadline = pq.top().first;
                    int process = pq.top().second.first;
                    int id = pq.top().second.second;
                    pq.pop();
            
                    orderOfOrder.push_back({id, curTime}); // for output

                    if (curTime > deadline) return -1;
                    curTime += process;
                }
            }
            if (printAns) {
                cout << tour[i] << " " << norder << " ";
                Time(arrivetime).printTime(" ");
                Time(curTime).printTime("\n");
                for (auto order:orderOfOrder) {
                    cout << order.first << " ";
                    Time(order.second).printTime("\n");
                }
            }
            // di chuyen den hub tiep theo
            int distance = instance.dist[tour[i]][tour[(i + 1) % tour.size()]];
            ret = curTime;
            curTime += ceil(distance / velocity);
            
        }
        if (curTime > endTime)
            return -1;
        // tro ve diem bat dau
        if (printAns) {
            cout << tour[0] << " 0 ";
            Time(curTime).printTime(" ");
            Time(curTime).printTime("\n");
        }
        // tra ve ket qua la thoi gian roi khoi thanh pho cuoi cung
        return ret;
    }

    int addItem(int orderId, int truckId, int force = 0) {
        // constrain ve weight va volume da duoc xi li truoc do
        Order &order = instance.orders[orderId];

        int uId = inTour(order.hubStart, truckId);
        int vId = inTour(order.hubEnd, truckId);

        // TH v tham sau u
        // TH v da xuat hien trong tour nhung u chua xuat hien
        if (uId > vId) return -1; 

        vector<int> oldTour = pickingPlan[truckId].tour; // for backup

        pickingPlan[truckId].addItem(orderId, instance);
        if (uId == __INT_MAX__) { // u va v deu chua xuat hien
            Picking &pickingplan = pickingPlan[truckId];
            pickingplan.tour.push_back(order.hubStart);
            pickingplan.tour.push_back(order.hubEnd);
            int tmp = evaluate(truckId);
            if (force) {
                orderState[orderId] = 1;
                pickingPlan[truckId].remainWeight -= instance.orders[orderId].weight;
                pickingPlan[truckId].remainVolume -= instance.orders[orderId].volume;
                return tmp;
            }
            // xi li sau
            pickingPlan[truckId].removeItem(orderId, instance);
            pickingplan.tour.pop_back();
            pickingplan.tour.pop_back();
            return tmp;
        }
        if (vId == __INT_MAX__) { // chi co v chua xuat hien
            int bestId = -1;
            int bestSol = __INT_MAX__;
            vector<int> newTour;
            vector<int> bestTour;
            for (int i = uId + 1; i <= oldTour.size(); i++) {
                newTour = oldTour;
                newTour.insert(newTour.begin() + i, order.hubEnd);
                pickingPlan[truckId].tour = newTour;
                int tmp = evaluate(truckId);
                if (tmp < bestSol) {
                    bestSol = tmp;
                    bestId = i;
                    if (force) {
                        bestTour = newTour;
                    }
                }
            }
            if (bestId == -1) return -1;

            if (force) {
                pickingPlan[truckId].tour = bestTour;
                pickingPlan[truckId].remainWeight -= instance.orders[orderId].weight;
                pickingPlan[truckId].remainVolume -= instance.orders[orderId].volume;
                orderState[orderId] = 1;
                return bestSol;
            } else {
                pickingPlan[truckId].tour = oldTour;
                pickingPlan[truckId].removeItem(orderId, instance);
            }
            return bestSol;
        } else { // ca u va v deu da xuat hien
            pickingPlan[truckId].pick[orderId].isPick = 1;
            int tmp = evaluate(truckId);
            if (force) {
                orderState[orderId] = 1;
                pickingPlan[truckId].remainWeight -= instance.orders[orderId].weight;
                pickingPlan[truckId].remainVolume -= instance.orders[orderId].volume;
            } else {
                pickingPlan[truckId].removeItem(orderId, instance);
            }
            return tmp;
        }
        return -1;        
    }

    void solve() {
        for (int truckId = 1; truckId <= instance.nTruck; truckId++) {
            Picking &pickingplan = pickingPlan[truckId];
            while (1) {
                int addId = 0; 
                int mincost = __INT_MAX__;
                for (int orderId = 1; orderId < instance.nOrder; orderId++) 
                if (orderState[orderId] == 0) {
                    Order &order = instance.orders[orderId];
                    if (pickingplan.remainVolume < order.volume) continue;
                    if (pickingplan.remainWeight < order.weight) continue;
                    int cost = addItem(orderId, truckId);
                    if (cost == -1) continue;
                    if (cost < mincost) {
                        mincost = cost;
                        addId = orderId;
                    }
                }
                if (addId == 0) break;
                // orderState[addId] = 1;
                addItem(addId, truckId, 1);
            }
        }
    }

    void printAnswer() {
        for (int truckId = 1; truckId <= instance.nTruck; truckId++) {
            // if (truckId != 8) continue;
            Picking &pickingplan = pickingPlan[truckId];
            if (pickingplan.tour.size() == 1) {
                cout << "1\n";
                cout << instance.truckes[truckId].idHub << " 0 ";
                Time t = instance.truckes[truckId].timeValid.start_;
                t.printTime(" ");
                t.printTime("\n");
            } else {
                cout << pickingplan.tour.size() + 1 << "\n";
                evaluate(truckId, 1);
            }
            // cout << endl;
            // return;
        }
    }

};

int main() {
    freopen("./INPUT/01.txt", "r", stdin);
    freopen("./OUTPUT/01.txt", "w", stdout);
    Instance instance;
    instance.readInstance();
    Solution sol(instance);
    sol.solve();
    sol.printAnswer();
    return 0;
}