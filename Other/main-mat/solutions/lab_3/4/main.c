#include "Mail.c"

int main()
{
    Post post;
    post.mails = (Mail **)malloc(sizeof(Mail *) * MIN_BUF);
    post.mails_cnt = 0;
    post._buffer = MIN_BUF;

    post.adress = (Adress *)malloc(sizeof(Adress) * MIN_BUF);
    post.adress->street = NULL;
    post.adress->city = NULL;
    post.adress->index = NULL;
    post.adress->segment = NULL;

    
    run(&post);
    delete_post(&post);
    return 0;
}