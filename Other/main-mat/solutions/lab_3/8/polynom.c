#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define ll long long


int max(int a, int b) { return (a > b) ? a : b; }

typedef enum
{
    correct,
    runtime_error,
    invalid_input,
    allocate_error,
    invalid_file,
    end_of,
} statements;

typedef struct Element Element;

struct Element
{
    Element *next;
    int value;
}; 

typedef struct
{
    Element *first;
    Element *last;
    int cnt;
}Polynom; 



Element* get_by_index(const Polynom *p, int index)
{
    Element *ptr = p->first;
    int i = 0;
    while (i < index)
    {
        ptr = ptr->next;
        i++;
    }
    return ptr;
}

statements create_polynom(Polynom **result)
{
    Polynom *p = (Polynom *) malloc(sizeof(Polynom));
    if (p == NULL) {return allocate_error; }

    p->cnt = 0;
    p->first = (Element *) malloc(sizeof(Element));
    if (p->first == NULL) {free(p); return allocate_error; }

    p->last = p->first;
    p->first->next = NULL;

    *result = p;
    return correct;
}

void clear_elements(Element *current)
{
    if (current == NULL) { return; }
    if (current->next != NULL)
    {
        clear_elements(current->next);
    }
    free(current);
    
}

statements delete_polynom(Polynom *p)
{
     if (p == NULL) {return invalid_input; }
     if (p->first != NULL) { clear_elements(p->first); }
     free(p);
     p = NULL;
}

void print_elements(Element *e)
{
    if (e == NULL) {return; }
    printf("%d ", e->value);
    if (e->next != NULL)
    {
        print_elements(e->next);
    }
}

void print_polynom(Polynom *p)
{
    if (p == NULL) { return; }
    printf("%d : ", p->cnt);
    print_elements(p->first);
    printf("\n");
}


statements calculate_polynom(const Polynom *p, ll *res, int x)
{
    if (p == NULL) {return invalid_input; }
    *res = 0;
    Element *ptr = p->first;
    for (int i = 0; i < p->cnt; i++)
    {
        if (ptr == NULL) {  break; }
        *res = (*res) * x;
        *res = (*res) + ptr->value;
        ptr = ptr->next;
        
    }
    return correct;
}


statements diff_polynom(Polynom *p)
{
    if (p == NULL) {return invalid_input; }
    if (p->cnt == 1)
    {
        p->first->value = 0;
        return correct;
    }

    Element *ptr = p->first;
    for (int i = 0; i < p->cnt-1; i++)
    {
        ptr->value = ptr->value * ((p->cnt) - i - 1);
        ptr = ptr->next;
    }
    
    Element *e = get_by_index(p, p->cnt - 2);
    clear_elements(ptr);
    e->next = NULL;
    (p->cnt) = (p->cnt) - 1;
    
    return correct;
}


statements add_koefficient(Polynom *p, int k)
{
    if (p->cnt == 0){
        p->cnt = p->cnt + 1;
        p->first->value = k;
        return correct;
    }
    Element *ptr = p->last;
    
    ptr->next = (Element *) malloc(sizeof(Element));
    if (ptr->next == NULL)
    {
        return allocate_error;
    }

    ptr->next->next = NULL;
    p->last = ptr->next;
    p->last->value = k;

    p->cnt = (p->cnt) + 1;

    return correct;
}


//size more then current cnt (not for usage)
statements resize_polynom(Polynom *p, int size)
{
    if (size < p->cnt ) {return invalid_input; }
    while (size > p->cnt)
    {
        statements stm = add_koefficient(p, 0);
        if (stm != correct) {return stm;}
    }
    return correct;
}



statements subscription_polynoms(const Polynom *a, const Polynom *b, Polynom **result)
{
    Polynom *p;
    statements stm = create_polynom(&p);
    if (stm != correct) { return stm; }
    int size = max(a->cnt, b->cnt);

    stm = resize_polynom(p, size);
    if (stm != correct) { return stm; }

    for (int i = a->cnt; i > 0; i--)
    {
        Element *_a = get_by_index(p, size - i);
        Element *_b = get_by_index(a, a->cnt - i);
        _a->value = _b->value;
        //p->koeffs[size - i] = (a->koeffs[a->cnt - i]);
    }
    for (int i = b->cnt; i > 0; i--)
    {
        Element *_a = get_by_index(p, size - i);
        Element *_b = get_by_index(b, b->cnt - i);
        _a->value =  _a->value - _b->value;
       //p->koeffs[size - i] =  p->koeffs[size - i] - (b->koeffs[b->cnt - i]);
    }
    *result = p;
    return correct;
}


statements summation_polynoms(const Polynom *a, const Polynom *b, Polynom **result)
{
    Polynom *p;
    statements stm = create_polynom(&p);
    if (stm != correct) { return stm; }
    int size = max(a->cnt, b->cnt);

    stm = resize_polynom(p, size);
    if (stm != correct) { return stm; }

    for (int i = a->cnt; i > 0; i--)
    {
        Element *_a = get_by_index(p, size - i);
        Element *_b = get_by_index(a, a->cnt - i);
        _a->value = _b->value;
       //p->koeffs[size - i] = (a->koeffs[a->cnt - i]);
    }

    for (int i = b->cnt; i > 0; i--)
    {
        Element *_a = get_by_index(p, size - i);
        Element *_b = get_by_index(b, b->cnt - i);
        _a->value =  _a->value + _b->value;
       //p->koeffs[size - i] = (b->koeffs[b->cnt - i]) +  p->koeffs[size - i];
    }

    *result = p;
    return correct;
}


statements multiply_polynoms(const Polynom *a, const Polynom *b, Polynom **result)
{
    
    Polynom *p;
    statements stm = create_polynom(&p);
    if (stm != correct) { return stm; }

    int size = a->cnt + b->cnt;
    if (a->cnt == 1 || 1 == b->cnt) { size--; }

    stm = resize_polynom(p, size);
    if (stm != correct) { return stm; }
    for (int i = 0; i < a->cnt; i++)
    {
        
        Element *_i = get_by_index(a, i);
        for (int j = 0; j < b->cnt; j++)
        {
            Element *_j = get_by_index(b, j);
            Element *_ij = get_by_index(p, i+j);
            _ij->value = _ij->value + (_i->value * _j->value);

            //p->koeffs[i+j] = (a->koeffs[i] * b->koeffs[j]) +  p->koeffs[i+j] ;
        }
    }
    *result = p;
    return correct;
}


statements devide_polynoms(const Polynom *a, const Polynom *b, Polynom **main_part, Polynom **remainder_part) 
{
    Polynom *main, *remainder;

    statements stm = create_polynom(&main);
    if (stm != correct) { return stm; }

    stm = create_polynom(&remainder);
    if (stm != correct) { delete_polynom(main); return stm; }

    int size = a->cnt;

    stm = resize_polynom(remainder, size);
    if (stm != correct) { delete_polynom(remainder); delete_polynom(main); return stm; }

    stm = resize_polynom(main, size);
    if (stm != correct) { delete_polynom(remainder); delete_polynom(main); return stm; }


    if (a->cnt < b->cnt) {return correct; } // a остаток, целой нет

    Element *ptr = a->first, *ptr2 = remainder->first;

    for (int i = 0; i < a->cnt; i++)
    {
        ptr2->value = ptr->value;
        ptr = ptr->next;
        ptr2 = ptr2->next;
    }
    ptr = remainder->first;
    ptr2 = main->first;

    
    for (int i = 0; i <= (a->cnt - b->cnt); i++) 
    {
        if (b->first->value == 0)
        {
            delete_polynom(remainder); delete_polynom(main);
            return invalid_input;
        }
        int coef = (int)(ptr->value / b->first->value);
        //printf("%d ", coef);
        ptr2->value= coef;
        Element *_j = b->first;
        for (int j = 0; j < b->cnt; j++)
        {
            Element *_ij = get_by_index(remainder, i+j);
            _ij->value = _ij->value - coef * _j->value;
            _j = _j->next;
            //remainder->koeffs[i + j] = coef * b->koeffs[j] - remainder->koeffs[i + j];
        }
        ptr = ptr->next;
        ptr2 = ptr2->next;
    }
    
    ptr = remainder->first;
    Element *endpoint = remainder->first;
    int cnt = 0;
    for (int i = 0; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->value != 0)
        {
            endpoint = ptr;
            cnt = i + 1;
        }
        i++;
    }
    remainder->cnt = cnt;
    clear_elements(endpoint->next);
    endpoint->next = NULL;
    
    if (remainder->cnt == 0)
    {
        delete_polynom(remainder);
        stm = create_polynom(&remainder);
        if (stm != correct) { delete_polynom(main); return stm; }

        stm = resize_polynom(remainder, 1);
        if (stm != correct) { delete_polynom(main); return stm; }
        remainder->cnt = 1;
    }
   
    cnt = 0;
    ptr = main->first;
    endpoint = main->first;

    for (int i = 0; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->value != 0)
        {
            endpoint = ptr;
            cnt = i + 1;
        }
        i++;
    }
    
    main->cnt = cnt; 
    clear_elements(endpoint->next);
     
    endpoint->next = NULL;

    if (main->cnt == 0)
    {
        delete_polynom(main);
        stm = create_polynom(&main);
        if (stm != correct) { delete_polynom(remainder); return stm; }

        stm = resize_polynom(main, 1);
        if (stm != correct) { delete_polynom(remainder); return stm; }
        main->cnt = 1;
    }

    *main_part = main;
    *remainder_part = remainder;
    //printf("\n");
    return correct;
}

statements div_polynoms(const Polynom *a, const Polynom *b, Polynom **main_part)
{
    Polynom *temp = NULL;
    statements stm = devide_polynoms(a, b, main_part, &temp);
    if (stm != correct) { return stm ;}
    delete_polynom(temp);
    return correct;
}

statements mod_polynoms(const Polynom *a, const Polynom *b, Polynom **reminder_part)
{
    Polynom *temp = NULL;
    statements stm = devide_polynoms(a, b, &temp, reminder_part);
    if (stm != correct) { return stm ;}
    delete_polynom(temp);
    return correct;
}

statements multiply_polynom_by_pow(const Polynom *a, int pow, int val, Polynom **result)
{
    Polynom *p;
    statements stm = create_polynom(&p);
    if (stm != correct) { return stm; }

    int size = a->cnt + pow;

    stm = resize_polynom(p, size);
    if (stm != correct) { return stm; }

    for (int i = 0; i < a->cnt; i++)
    {
        Element *_i = get_by_index(p, i);
        Element *_j = get_by_index(a, i);
        _i->value = (_j->value * val);
    }
    *result = p;
    return correct;
}

statements compose_polynoms(const Polynom *A, const Polynom *B, Polynom **resulting) {
    Polynom *result, *temp;
    Element *ptr = A->first;
    statements stm;

    stm = create_polynom(&result);
    if (stm != correct) {return stm; }
    
    for (int i = A->cnt-1; i >= 0; i--)
    {
        Polynom *term = NULL;
        //stm = create_polynom(&term);
        //if (stm != correct) {delete_polynom(result); return stm; }
        stm = multiply_polynom_by_pow(B, i, ptr->value, &term);
        if (stm != correct) {delete_polynom(result); delete_polynom(term); return stm; }
        //printf("%d \n", ptr->value);

        stm = summation_polynoms(result, term, &temp);
        if (stm != correct) {delete_polynom(result); delete_polynom(term); return stm; }
        delete_polynom(result);
        result = temp;
        delete_polynom(term);
        ptr = ptr->next;
    }
    
    *resulting = result;
    return correct;
}
void swap(Polynom **a, Polynom **b){
    Polynom **temp = a;
    a = b;
    b = temp;
}