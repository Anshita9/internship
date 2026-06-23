#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "func.h"

#define MAX 40
#define TOKEN_SIZE 16


// ---------------- STACKS ----------------

typedef struct {
    char data[MAX][TOKEN_SIZE];
    int top;
} S_Str;


typedef struct {
    double data[MAX];
    int top;
} S_Dbl;



void push_s(S_Str *s, const char *v)
{
    if (s->top >= MAX-1) return;
    strcpy(s->data[++s->top], v);
}


char* pop_s(S_Str *s)
{
    if(s->top < 0) return "";
    return s->data[s->top--];
}


char* peek_s(S_Str *s)
{
    if(s->top < 0) return "";
    return s->data[s->top];
}



void push_d(S_Dbl *s,double v)
{
    if(s->top >= MAX-1) return;
    s->data[++s->top]=v;
}


double pop_d(S_Dbl *s)
{
    if(s->top < 0) return 0.0;
    return s->data[s->top--];
}



// ---------------- HELPERS ----------------


int is_op(const char *t)
{
    return (!strcmp(t,"+") ||
            !strcmp(t,"-") ||
            !strcmp(t,"*") ||
            !strcmp(t,"/") ||
            !strcmp(t,"^") ||
            !strcmp(t,"!"));
}



int is_func(const char *t)
{
    return (!strcmp(t,"sin") ||
            !strcmp(t,"cos") ||
            !strcmp(t,"tan") ||
            !strcmp(t,"ln") ||
            !strcmp(t,"sqrt") ||
            !strcmp(t,"fact") ||
            !strcmp(t,"exp") ||
            !strcmp(t,"log10") ||
            !strcmp(t,"asin") ||
            !strcmp(t,"acos") ||
            !strcmp(t,"atan") ||
            !strcmp(t,"mod"));
}



int prec(const char *op)
{
    if(!strcmp(op,"!")) return 5;
    if(!strcmp(op,"^")) return 4;
    if(!strcmp(op,"*") || !strcmp(op,"/")) return 3;
    if(!strcmp(op,"+") || !strcmp(op,"-")) return 2;

    return 0;
}



// ---------------- TOKENIZER ----------------
//
// sin7  -> sin 7
// cos45 -> cos 45
// ln10  -> ln 10
//

void tokenize(char *in,char *out)
{

    int j=0;


    for(int i=0; in[i]!='\0' && j<MAX*10-2; i++)
    {

        char c=in[i];


        // operators and brackets
        if(strchr("()+-*/^!",c))
        {
            out[j++]=' ';
            out[j++]=c;
            out[j++]=' ';
        }



        // functions
        else if(isalpha(c))
        {

            out[j++]=' ';


            while(isalpha(in[i]))
            {
                out[j++]=in[i++];
            }


            i--;


            out[j++]=' ';
        }



        // numbers
        else
        {
            out[j++]=c;
        }

    }


    out[j]='\0';

}



// ---------------- SHUNTING YARD ----------------


void shunt(char *input,
           char output[MAX][TOKEN_SIZE],
           int *out_len)
{

    S_Str ops;

    ops.top=-1;


    char expr[MAX*10];


    tokenize(input,expr);



    char *token=strtok(expr," \t\n");

    *out_len=0;



    while(token)
    {


        // number
        if(isdigit(token[0]) ||
          (token[0]=='-' && isdigit(token[1])))
        {

            strcpy(output[(*out_len)++],token);

        }


        // function or (
        else if(is_func(token) ||
                !strcmp(token,"("))
        {

            push_s(&ops,token);

        }


        // )
        else if(!strcmp(token,")"))
        {

            while(ops.top>=0 &&
                 strcmp(peek_s(&ops),"("))
            {
                strcpy(output[(*out_len)++],
                       pop_s(&ops));
            }


            if(ops.top>=0)
                pop_s(&ops);



            if(ops.top>=0 &&
               is_func(peek_s(&ops)))
            {
                strcpy(output[(*out_len)++],
                       pop_s(&ops));
            }

        }



        // operators
        else
        {

            while(ops.top>=0 &&
             prec(peek_s(&ops))>=prec(token))
            {
                strcpy(output[(*out_len)++],
                       pop_s(&ops));
            }


            push_s(&ops,token);

        }



        token=strtok(NULL," \t\n");

    }



    while(ops.top>=0)
    {
        strcpy(output[(*out_len)++],
               pop_s(&ops));
    }

}



// ---------------- RPN EVALUATION ----------------


double eval_rpn(char rpn[MAX][TOKEN_SIZE],
                int n,
                double x)
{

    S_Dbl st;

    st.top=-1;



    for(int i=0;i<n;i++)
    {

        char *t=rpn[i];



        if(isdigit(t[0]) ||
          (t[0]=='-' && isdigit(t[1])))
        {

            push_d(&st,atof(t));

        }



        else if(is_op(t))
        {

            if(!strcmp(t,"!"))
            {
                double a=pop_d(&st);
                push_d(&st,m_fact(a));
            }


            else
            {

                double b=pop_d(&st);
                double a=pop_d(&st);


                if(!strcmp(t,"+"))
                    push_d(&st,a+b);

                else if(!strcmp(t,"-"))
                    push_d(&st,a-b);

                else if(!strcmp(t,"*"))
                    push_d(&st,a*b);

                else if(!strcmp(t,"/"))
                    push_d(&st,b?a/b:0);

                else if(!strcmp(t,"^"))
                    push_d(&st,m_pow(a,b));

            }

        }



        else if(is_func(t))
        {

            double a=pop_d(&st);



            if(!strcmp(t,"sin"))
                push_d(&st,m_sin(a));

            else if(!strcmp(t,"cos"))
                push_d(&st,m_cos(a));

            else if(!strcmp(t,"tan"))
                push_d(&st,m_tan(a));

            else if(!strcmp(t,"ln"))
                push_d(&st,m_ln(a));

            else if(!strcmp(t,"sqrt"))
                push_d(&st,m_sqrt(a));

            else if(!strcmp(t,"fact"))
                push_d(&st,m_fact(a));

            else if(!strcmp(t,"exp"))
                push_d(&st,m_exp(a));

            else if(!strcmp(t,"log10"))
                push_d(&st,m_log10(a));

            else if(!strcmp(t,"asin"))
                push_d(&st,m_asin(a));

            else if(!strcmp(t,"acos"))
                push_d(&st,m_acos(a));

            else if(!strcmp(t,"atan"))
                push_d(&st,m_atan(a));

            else if(!strcmp(t,"mod"))
                push_d(&st,m_abs(a));

        }

    }


    return pop_d(&st);

}



// ---------------- MAIN API ----------------


void compute(const char *input,
             double start,
             double end,
             double step,
             char *output)
{


    char rpn[MAX][TOKEN_SIZE];

    int n=0;



    shunt((char*)input,rpn,&n);



    output[0]='\0';



    if(step<=0 || m_abs(end-start)<1e-9)
    {

        double res=eval_rpn(rpn,n,start);

        sprintf(output,"%.6f",res);

    }


    else
    {

        char line[64];

        double t=start;

        int safety=0;



        while(t<=end+1e-9 && safety++<2000)
        {

            double res=eval_rpn(rpn,n,t);

            sprintf(line,"%.4f,%.4f\n",
                    t,res);

            strcat(output,line);

            t+=step;
        }

    }

}
