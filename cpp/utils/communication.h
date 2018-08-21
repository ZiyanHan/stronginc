#ifndef CORE_COMMUNICATION_H_
#define CORE_COMMUNICATION_H_

#include <mpi.h>
#include "time.h"
#include "serialization.h"
#include "cpp/core/global.h"
//============================================
//Allreduce
int all_sum(int my_copy);

///* Record the stat of memory and message*/
//long sum_stat(long my_copy){
//	long tmp;
//	MPI_Reduce(&my_copy, &tmp, 1, MPI_LONG_INT,MPI_SUM, MASTER_RANK,MPI_COMM_WORLD);
//	return tmp;
//}
long long master_sum_LL(long long my_copy);

int master_sum(int my_copy) ;

long long all_sum_LL(long long my_copy) ;

char all_bor(char my_copy) ;

/*
 bool all_lor(bool my_copy){
 bool tmp;
 MPI_Allreduce(&my_copy, &tmp, 1, MPI_BYTE, MPI_LOR, MPI_COMM_WORLD);
 return tmp;
 }
 */

//============================================
//char-level send/recv
void pregel_send(void *buf, int size, int dst, unsigned tag = 0);

void pregel_recv(void *buf, int size, int src, unsigned tag = 0);

//============================================
//binstream-level send/recv
void send_ibinstream(ibinstream &m, int dst, unsigned tag = 0) ;

obinstream recv_obinstream(int src, unsigned tag = 0);

//============================================
//obj-level send/recv
template<class T>
void send_data(const T &data, int dst) {
    ibinstream m;
    m << data;
    send_ibinstream(m, dst);
}

template<class T>
T recv_data(int src) {
    obinstream um = recv_obinstream(src);
    T data;
    um >> data;
    return data;
}

//============================================
//all-to-all
template<class T>
void all_to_all(std::vector <T> &to_exchange, unsigned tag = 0) {
    StartTimer(COMMUNICATION_TIMER);
    //for each to_exchange[i]
    //        send out *to_exchange[i] to i
    //        save received data in *to_exchange[i]
    int np = get_num_workers();
    int me = get_worker_id();
    for (int i = 0; i < np; i++) {
        int partner = (i - me + np) % np;
        if (me != partner) {
            if (me < partner) {
                StartTimer(SERIALIZATION_TIMER);
                //send
                ibinstream m;
                m << to_exchange[partner];
                StopTimer(SERIALIZATION_TIMER);
                StartTimer(TRANSFER_TIMER);
                send_ibinstream(m, partner, tag);
                StopTimer(TRANSFER_TIMER);
                //receive
                StartTimer(TRANSFER_TIMER);
                obinstream um = recv_obinstream(partner, tag);
                StopTimer(TRANSFER_TIMER);
                StartTimer(SERIALIZATION_TIMER);
                um >> to_exchange[partner];
                StopTimer(SERIALIZATION_TIMER);
            } else {
                StartTimer(TRANSFER_TIMER);
                //receive
                obinstream um = recv_obinstream(partner, tag);
                StopTimer(TRANSFER_TIMER);
                StartTimer(SERIALIZATION_TIMER);
                T received;
                um >> received;
                //send
                ibinstream m;
                m << to_exchange[partner];
                StopTimer(SERIALIZATION_TIMER);
                StartTimer(TRANSFER_TIMER);
                send_ibinstream(m, partner, tag);
                StopTimer(TRANSFER_TIMER);
                to_exchange[partner] = received;
            }
        }
    }
    StopTimer(COMMUNICATION_TIMER);
}

template<class T, class T1>
void all_to_all_cat(std::vector <T> &to_exchange1,
                    std::vector <T1> &to_exchange2) {
    StartTimer(COMMUNICATION_TIMER);
    //for each to_exchange[i]
    //        send out *to_exchange[i] to i
    //        save received data in *to_exchange[i]
    int np = get_num_workers();
    int me = get_worker_id();
    for (int i = 0; i < np; i++) {
        int partner = (i - me + np) % np;
        if (me != partner) {
            if (me < partner) {
                StartTimer(SERIALIZATION_TIMER);
                //send
                ibinstream m;
                m << to_exchange1[partner];
                m << to_exchange2[partner];
                StopTimer(SERIALIZATION_TIMER);
                StartTimer(TRANSFER_TIMER);
                send_ibinstream(m, partner);
                StopTimer(TRANSFER_TIMER);
                //receive
                StartTimer(TRANSFER_TIMER);
                obinstream um = recv_obinstream(partner);
                StopTimer(TRANSFER_TIMER);
                StartTimer(SERIALIZATION_TIMER);
                um >> to_exchange1[partner];
                um >> to_exchange2[partner];
                StopTimer(SERIALIZATION_TIMER);
            } else {
                StartTimer(TRANSFER_TIMER);
                //receive
                obinstream um = recv_obinstream(partner);
                StopTimer(TRANSFER_TIMER);
                StartTimer(SERIALIZATION_TIMER);
                T received1;
                T1 received2;
                um >> received1;
                um >> received2;
                //send
                ibinstream m;
                m << to_exchange1[partner];
                m << to_exchange2[partner];
                StopTimer(SERIALIZATION_TIMER);
                StartTimer(TRANSFER_TIMER);
                send_ibinstream(m, partner);
                StopTimer(TRANSFER_TIMER);
                to_exchange1[partner] = received1;
                to_exchange2[partner] = received2;
            }
        }
    }
    StopTimer(COMMUNICATION_TIMER);
}

template<class T, class T1, class T2>
void all_to_all_cat(std::vector <T> &to_exchange1, std::vector <T1> &to_exchange2,
                    std::vector <T2> &to_exchange3) {
    StartTimer(COMMUNICATION_TIMER);
    //for each to_exchange[i]
    //        send out *to_exchange[i] to i
    //        save received data in *to_exchange[i]
    int np = get_num_workers();
    int me = get_worker_id();
    for (int i = 0; i < np; i++) {
        int partner = (i - me + np) % np;
        if (me != partner) {
            if (me < partner) {
                StartTimer(SERIALIZATION_TIMER);
                //send
                ibinstream m;
                m << to_exchange1[partner];
                m << to_exchange2[partner];
                m << to_exchange3[partner];
                StopTimer(SERIALIZATION_TIMER);
                StartTimer(TRANSFER_TIMER);
                send_ibinstream(m, partner);
                StopTimer(TRANSFER_TIMER);
                //receive
                StartTimer(TRANSFER_TIMER);
                obinstream um = recv_obinstream(partner);
                StopTimer(TRANSFER_TIMER);
                StartTimer(SERIALIZATION_TIMER);
                um >> to_exchange1[partner];
                um >> to_exchange2[partner];
                um >> to_exchange3[partner];
                StopTimer(SERIALIZATION_TIMER);
            } else {
                StartTimer(TRANSFER_TIMER);
                //receive
                obinstream um = recv_obinstream(partner);
                StopTimer(TRANSFER_TIMER);
                StartTimer(SERIALIZATION_TIMER);
                T received1;
                T1 received2;
                T2 received3;
                um >> received1;
                um >> received2;
                um >> received3;
                //send
                ibinstream m;
                m << to_exchange1[partner];
                m << to_exchange2[partner];
                m << to_exchange3[partner];
                StopTimer(SERIALIZATION_TIMER);
                StartTimer(TRANSFER_TIMER);
                send_ibinstream(m, partner);
                StopTimer(TRANSFER_TIMER);
                to_exchange1[partner] = received1;
                to_exchange2[partner] = received2;
                to_exchange3[partner] = received3;
            }
        }
    }
    StopTimer(COMMUNICATION_TIMER);
}

template<class T, class T1>
void all_to_all(std::vector <T> &to_send, std::vector <T1> &to_get) {
    StartTimer(COMMUNICATION_TIMER);
    //for each to_exchange[i]
    //        send out *to_exchange[i] to i
    //        save received data in *to_exchange[i]
    int np = get_num_workers();
    int me = get_worker_id();
    for (int i = 0; i < np; i++) {
        int partner = (i - me + np) % np;
        if (me != partner) {
            if (me < partner) {
                StartTimer(SERIALIZATION_TIMER);
                //send
                ibinstream m;
                m << to_send[partner];
                StopTimer(SERIALIZATION_TIMER);
                StartTimer(TRANSFER_TIMER);
                send_ibinstream(m, partner);
                StopTimer(TRANSFER_TIMER);
                //receive
                StartTimer(TRANSFER_TIMER);
                obinstream um = recv_obinstream(partner);
                StopTimer(TRANSFER_TIMER);
                StartTimer(SERIALIZATION_TIMER);
                um >> to_get[partner];
                StopTimer(SERIALIZATION_TIMER);
            } else {
                StartTimer(TRANSFER_TIMER);
                //receive
                obinstream um = recv_obinstream(partner);
                StopTimer(TRANSFER_TIMER);
                StartTimer(SERIALIZATION_TIMER);
                T1 received;
                um >> received;
                //send
                ibinstream m;
                m << to_send[partner];
                StopTimer(SERIALIZATION_TIMER);
                StartTimer(TRANSFER_TIMER);
                send_ibinstream(m, partner);
                StopTimer(TRANSFER_TIMER);
                to_get[partner] = received;
            }
        }
    }
    StopTimer(COMMUNICATION_TIMER);
}

//============================================
//scatter
template<class T>
void masterScatter(std::vector <T> &to_send) { //scatter
    StartTimer(COMMUNICATION_TIMER);
    int *sendcounts = new int[_num_workers];
    int recvcount;
    int *sendoffset = new int[_num_workers];

    ibinstream m;
    StartTimer(SERIALIZATION_TIMER);
    int size = 0;
    for (int i = 0; i < _num_workers; i++) {
        if (i == _my_rank) {
            sendcounts[i] = 0;
        } else {
            m << to_send[i];
            sendcounts[i] = m.size() - size;
            size = m.size();
        }
    }
    StopTimer(SERIALIZATION_TIMER);

    StartTimer(TRANSFER_TIMER);
    MPI_Scatter(sendcounts, 1, MPI_INT, &recvcount, 1, MPI_INT, MASTER_RANK,
                MPI_COMM_WORLD);
    StopTimer(TRANSFER_TIMER);

    for (int i = 0; i < _num_workers; i++) {
        sendoffset[i] = (i == 0 ? 0 : sendoffset[i - 1] + sendcounts[i - 1]);
    }
    char *sendbuf = m.get_buf(); //ibinstream will delete it
    char *recvbuf;

    StartTimer(TRANSFER_TIMER);
    MPI_Scatterv(sendbuf, sendcounts, sendoffset, MPI_CHAR, recvbuf, recvcount,
                 MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD);
    StopTimer(TRANSFER_TIMER);

    delete[] sendcounts;
    delete[] sendoffset;
    StopTimer(COMMUNICATION_TIMER);
}

template<class T>
void slaveScatter(T &to_get) { //scatter
    StartTimer(COMMUNICATION_TIMER);
    int *sendcounts;
    int recvcount;
    int *sendoffset;

    StartTimer(TRANSFER_TIMER);
    MPI_Scatter(sendcounts, 1, MPI_INT, &recvcount, 1, MPI_INT, MASTER_RANK,
                MPI_COMM_WORLD);

    char *sendbuf;
    char *recvbuf = new char[recvcount]; //obinstream will delete it

    MPI_Scatterv(sendbuf, sendcounts, sendoffset, MPI_CHAR, recvbuf, recvcount,
                 MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD);
    StopTimer(TRANSFER_TIMER);

    StartTimer(SERIALIZATION_TIMER);
    obinstream um(recvbuf, recvcount);
    um >> to_get;
    StopTimer(SERIALIZATION_TIMER);
    StopTimer(COMMUNICATION_TIMER);
}

//================================================================
//gather
template<class T>
void masterGather(std::vector <T> &to_get) { //gather
    StartTimer(COMMUNICATION_TIMER);
    int sendcount = 0;
    int *recvcounts = new int[_num_workers];
    int *recvoffset = new int[_num_workers];

    StartTimer(TRANSFER_TIMER);
    MPI_Gather(&sendcount, 1, MPI_INT, recvcounts, 1, MPI_INT, MASTER_RANK,
               MPI_COMM_WORLD);
    StopTimer(TRANSFER_TIMER);

    for (int i = 0; i < _num_workers; i++) {
        recvoffset[i] = (i == 0 ? 0 : recvoffset[i - 1] + recvcounts[i - 1]);
    }

    char *sendbuf;
    int recv_tot = recvoffset[_num_workers - 1] + recvcounts[_num_workers - 1];
    char *recvbuf = new char[recv_tot]; //obinstream will delete it

    StartTimer(TRANSFER_TIMER);
    MPI_Gatherv(sendbuf, sendcount, MPI_CHAR, recvbuf, recvcounts, recvoffset,
                MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD);
    StopTimer(TRANSFER_TIMER);

    StartTimer(SERIALIZATION_TIMER);
    obinstream um(recvbuf, recv_tot);
    for (int i = 0; i < _num_workers; i++) {
        if (i == _my_rank)
            continue;
        um >> to_get[i];
    }
    StopTimer(SERIALIZATION_TIMER);
    delete[] recvcounts;
    delete[] recvoffset;
    StopTimer(COMMUNICATION_TIMER);
}

template<class T>
void slaveGather(T &to_send) { //gather
    StartTimer(COMMUNICATION_TIMER);
    int sendcount;
    int *recvcounts;
    int *recvoffset;

    StartTimer(SERIALIZATION_TIMER);
    ibinstream m;
    m << to_send;
    sendcount = m.size();
    StopTimer(SERIALIZATION_TIMER);

    StartTimer(TRANSFER_TIMER);
    MPI_Gather(&sendcount, 1, MPI_INT, recvcounts, 1, MPI_INT, MASTER_RANK,
               MPI_COMM_WORLD);
    StopTimer(TRANSFER_TIMER);

    char *sendbuf = m.get_buf(); //ibinstream will delete it
    char *recvbuf;

    StartTimer(TRANSFER_TIMER);
    MPI_Gatherv(sendbuf, sendcount, MPI_CHAR, recvbuf, recvcounts, recvoffset,
                MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD);
    StopTimer(TRANSFER_TIMER);
    StopTimer(COMMUNICATION_TIMER);
}

//================================================================
//bcast
template<class T>
void masterBcast(T &to_send) { //broadcast
    StartTimer(COMMUNICATION_TIMER);

    StartTimer(SERIALIZATION_TIMER);
    ibinstream m;
    m << to_send;
    int size = m.size();
    StopTimer(SERIALIZATION_TIMER);

    StartTimer(TRANSFER_TIMER);
    MPI_Bcast(&size, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    char *sendbuf = m.get_buf();
    MPI_Bcast(sendbuf, size, MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD);
    StopTimer(TRANSFER_TIMER);

    StopTimer(COMMUNICATION_TIMER);
}

template<class T>
void slaveBcast(T &to_get) { //broadcast
    StartTimer(COMMUNICATION_TIMER);

    int size;

    StartTimer(TRANSFER_TIMER);
    MPI_Bcast(&size, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    StopTimer(TRANSFER_TIMER);

    StartTimer(TRANSFER_TIMER);
    char *recvbuf = new char[size]; //obinstream will delete it
    MPI_Bcast(recvbuf, size, MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD);
    StopTimer(TRANSFER_TIMER);

    StartTimer(SERIALIZATION_TIMER);
    obinstream um(recvbuf, size);
    um >> to_get;
    StopTimer(SERIALIZATION_TIMER);

    StopTimer(COMMUNICATION_TIMER);
}

#endif /* CORE_COMMUNICATION_H_ */

