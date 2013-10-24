#include<iostream>
#include<cstdio>
#include<map>
#include<string>
#include<fstream>
#include<stdexcept>

using namespace std;

//globally defined Variable
map<string, int> format;
map <string, string> opcode;
map<string,int>mnemonic;
map<string,string>littab;
string literal[100];

map <string,string> symbtab;
string lst_adr,base="0000";
string modification[100];
int ct=0,ct2=0,last_ltorg=0;

//Functions defined
string maketol(string in,int l)
{
    while(in.length()<l)
    {
        in="0"+in;
    }
    return in;
}
bool iscomment(string s)
{
    int i=0,n=s.length();
    while(s[i]<33&&i<n-1)i++;
    if(s[i]=='.')return true;
    return false;
}
bool find_comma(string str)
{
    int l=str.length();
    for(int i=0;i<l;i++)
    {
        if(str[i]==',')return true;
    }
    return false;
}
long long hex_dec(string str)
{

    int l=str.length();
    int position;
    long long ans=0;
    for(int i=0;i<l&&str[i]>=48;i++)
    {
            if(str[i]>=65)position=str[i]-65+10;
            else position=str[i]-48;
            ans=ans*16+position;
    }
    return ans;
}
string dec_hex(long long a)
{
    string ans="";
    int position;
    char ch;
    while(a)
    {
        position=a%16;
        a=a/16;
        if(position<=9)
        ch=(char)(position+48);
        else
        ch=(char)(position+55);
        ans=ch+ans;
    }
    return ans;
}
bool pc_is_possible(int idisp)
{
    if(idisp<=2047&&idisp>=(-2048))return 1;
    else return 0;
}
long long dec_dec1(string str)
{
    int n=str.length();
    long long ans=0,temp;
    for(int i=0;i<n&&str[i]>=48;i++)
    {

        temp=str[i]-48;
        ans=ans*10+temp;
    }
    return ans;
}

void form_mnemonic()
{
    string in1;
    int in2=5;
    ifstream in("mnemonic.txt");
    while(in2!=6)
    {
        in>>in1;
        in>>in2;
        mnemonic[in1]=in2;
    }
}
void form_optab()
{
    ifstream in("Opcode.txt");
    string str1,str3;
    int str2;
    while(str3[0]!='D'||str3[1]!='C')
    {
        in>>str1;
        in>>str2;
        in>>str3;
        format[str1]=str2;
        opcode[str1]=str3;
    }
}

void pass1()
{
    ifstream in("input_literal.txt");//change the file according to you need to compile
    ofstream out("intermediate.txt");
    string str,str1,str2,str3;
    int LOCTR=0,counter=0,ft=0;
    string loctr;
    do
    {
        getline(in,str);
        int i=0;
        int l=str.length();
        if(!iscomment(str))
        {
                counter++;
                str1="";str2="";str3="";
                while(str[i]!=' ')
                {
                    str1=str1+str[i];
                    i++;
                }
                if(str1.length()==0)str1="$";
                while(str[i]==' ')i++;
                while((str[i]!=' ')&&(i<l))
                {
                    str2=str2+str[i];
                    i++;
                }
                if(str2[str2.length() - 1] < 32)
                {
                    str2 = str2.substr(0, str2.length() - 1);
                }
                if(str1.length()==0)cout<<"There is error in code.";
                while(str[i]==' '&&i<l)i++;
                while(str[i]!=' '&&i<l)
                {
                    str3=str3+str[i];
                    i++;
                }
                if(str3[str3.length() - 1] < 32)
                {
                    str3 = str3.substr(0, str3.length() - 1);
                }
                if(str3.length()==0)str3="$";
                out<<str1<<endl;
                out<<str2<<endl;
                if(str3[0] == '=') {
                    out << "LITERAL" + dec_hex(ct2) << endl;
                } else {
                    out<<str3<<endl;
                }


                if(counter==1)
                {
                    loctr=str3;
                    LOCTR=hex_dec(loctr);
                    loctr=maketol(loctr,4);
                    ft=0;
                    //out<<loctr<<endl;
                }

                if(str1.length()==1&&str1[0]=='$');
                else
                {
                        symbtab[str1]=loctr;
                        //cout<<str1<<"  "<<symbtab[str1]<<endl;
                }
                out<<loctr<<endl;
                lst_adr=loctr;
                if(counter==1);
                else
                {
                    if(str2=="BYTE")
                    {
                        ft=0;
                        if(str3[0]=='X')LOCTR+=(str3.length()-4+1)/2;
                        else if(str3[0]=='C')LOCTR=LOCTR+(str3.length()-4);
                        else cout<<"error 1"<<endl;
                    }
                    else if(str2=="RESB")
                    {
                        LOCTR=LOCTR+dec_dec1(str3);
                        ft=0;
                    }
                    else if(str2=="WORD")
                    {
                        LOCTR=LOCTR+3;
                        ft=0;
                    }
                    else if(str2=="RESW")
                    {
                        LOCTR=LOCTR+3*dec_dec1(str3);
                        ft=0;
                    }
                    else if(str2[0]=='+')
                    {
                        LOCTR=LOCTR+4;
                        ft=4;
                    }
                    else if (str3[0]=='=')
                    {
                        LOCTR=LOCTR+format[str2];
                        littab["LITERAL"+dec_hex(ct2)]=str3.substr(1);
                        //cout<<ct2<<"LITERAL"<<dec_hex(ct2+1)<<endl;
                        ct2++;
                    }
                    else if(str2.substr(0,5)=="LTORG")
                    {
                        out<<"0"<<endl;
                        for(int i = last_ltorg; i < ct2; ++i)
                        {
                            symbtab["LITERAL" + dec_hex(i)] = dec_hex(LOCTR);
                            string data = littab["LITERAL" + dec_hex(i)];
                            out << "LITERAL" + dec_hex(i) <<  endl;
                            out << "BYTE" << endl;
                            out << data << endl;
                            out << dec_hex(LOCTR) << endl;
                            if(i != ct2 - 1)
                            {
                                out << "0" << endl;
                            }
                            if(data[0] == 'C')
                            {
                                LOCTR = LOCTR + (data.length() - 3);
                            } else {
                                LOCTR = LOCTR + (data.length() - 3)/2;
                            }
                        }
                        last_ltorg = ct2;
                        ft=0;
                    }
                    else
                    {
                        ft=format[str2];
                        if(str2[0]=='R'&&str2[1]=='S')ft=3;
                        LOCTR=LOCTR+ft;
                    }
                    loctr=dec_hex(LOCTR);
                    loctr=maketol(loctr,4);
                }
                out<<ft<<endl;
        }
        else
        {
           ;     //out<<str<<endl;
        }
    }
    while(str2!="END");
}
void pass2()
{
    ifstream in("intermediate.txt");
    ofstream out("output.txt");
    string str="",str1,str2,str3,form,laddr,adr="",start,len,fes;
    in>>str1;
    in>>str2;
    in>>str3;
    in>>laddr;
    in>>form;

    str3=dec_hex(hex_dec(lst_adr)-hex_dec(laddr));

    while(str3.length()<6){str3="0"+str3;}
    while(laddr.length()<6){laddr="0"+laddr;}

    out<<"H"<<str1<<laddr<<str3<<endl;
    int counter=0,flag=0,flag1=0;
    fes=laddr;
    str="";

    string prog_ctr,disp,ans;
    int pgct,idisp,ibase,x,b,p,e;


    string padr;
    flag1=0;counter=0;flag=0;
    do
    {
        in>>str1;
        in>>str2;
        in>>str3;
        in>>laddr;
        in>>form;

        if(flag==0)
        {
            laddr=maketol(laddr,6);
            start=laddr;
            flag=1;
        }



        if(str2=="BYTE")
        {
            flag1=0;
            int a;
            if(str3[0]=='C')
            {
                int l=str3.length();
                for(int i=2;i<l-1;i++)
                {
                    str=str+dec_hex(str3[i]);
                }
                a=str.length();
            }
            else
            {
                int l=str3.length();
                str=str3.substr(2,l-3);
                a=(str.length()+1)/2;
            }


            if(counter<30)
            {
                adr=adr+str;
                counter=counter+a;
            }
            if(counter<30);
            else
            {
                adr=str;
                counter=counter+a;
                len=dec_hex(hex_dec(laddr)-hex_dec(start));
                out<<"T"<<start<<len<<padr<<endl;
                //flag=0;
                counter=3;
                //adr="";
                start=dec_hex(hex_dec(laddr));
                start=maketol(start,6);
            }
        }
        else if(str2=="WORD")
        {
            flag1=0;
            long long i=dec_dec1(str3);
            str=dec_hex(i);
            int l=str.length();
            while((l)<6)
            {
                str="0"+str;
                l=str.length();
            }
            if(counter<28)
            {
                adr=adr+str;
                counter+=3;
            }
            else
            {
                len=dec_hex(hex_dec(laddr)-hex_dec(start));
                out<<"T"<<start<<len<<adr<<endl;
                //flag=0;
                counter=3;
                adr=str;
                start=dec_hex(hex_dec(laddr));
                start=maketol(start,6);
            }
        }
        else if(str2=="RESB"||str2=="RESW")
        {
            str="";
            if(flag1==0)
            {
                len=dec_hex(hex_dec(laddr)-hex_dec(start));
                out<<"T"<<start<<len<<adr<<endl;
                flag=0;
                counter=0;
                flag1=1;
                adr="";
            }
            if(str2=="RESB")
            {
                start=dec_hex(hex_dec(laddr)+dec_dec1(str3));
            }
            else
            {
                start=dec_hex(hex_dec(laddr)+3*dec_dec1(str3));
            }
            start=maketol(start,6);
            if(flag1==0);else continue;
            //lse continue;
        }
        else if(str2=="BASE")
        {
            ibase=hex_dec(symbtab[str3]);
            str=dec_hex(ibase);
            str="";
            continue;
        }
        else if(str2=="END")
        {
            flag1=0;
            str="";
            if(adr!="")
            {
                len=dec_hex(hex_dec(laddr)-hex_dec(start));
                out<<"T"<<start<<len<<adr<<endl;
            }
            for(int i=0;i<ct;i++)
            {
                out<<modification[i]<<endl;
            }
            out<<"E"<<fes<<endl;
        }
        else if(str2=="RSUB")
        {
            flag1=0;
            str="4F0000";
            if(counter<28)
            {
                adr=adr+str;
                counter=counter+3;
            }
            else
            {
                //adr=adr+str;
                len=dec_hex(hex_dec(laddr)-hex_dec(start));
                out<<"T"<<start<<len<<adr<<endl;
                //flag=0;
                counter=3;
                adr=str;
                start=dec_hex(hex_dec(laddr));
                start=maketol(start,6);
            }
            flag1=0;
        }
        else if(form=="1")
        {
            str=opcode[str2];
            if(counter<=29)
            {
                adr=adr+str;
                counter++;
            }
            else
            {
                //adr=adr+str;
                len=dec_hex(hex_dec(laddr)-hex_dec(start));
                out<<"T"<<start<<len<<adr<<endl;
                //flag=0;
                counter=1;
                adr=str;
                start=dec_hex(hex_dec(laddr));
                start=maketol(start,6);
            }
        }
        else if(form=="2")
        {
            string a,b;
            int a1,b1;
            a=str3[0];
            b=str3[2];
            if(str3.length()==1){b1=0;}
            else b1=mnemonic[b];
            a1=mnemonic[a];
            char ac=a1+48,bc=b1+48;
            //cout<<opcode[str2]<<endl;
            str=opcode[str2]+ac+bc;
            if(counter<=28)
            {
                adr=adr+str;
                counter=counter+2;
            }
            else
            {
                //adr=adr+str;
                len=dec_hex(hex_dec(laddr)-hex_dec(start));
                out<<"T"<<start<<len<<adr<<endl;
                //flag=0;
                counter=2;
                adr=str;
                start=dec_hex(hex_dec(laddr));
                start=maketol(start,6);
            }
            //cout<<str<<endl;
        }
        else if(form=="3")
        {
            if(find_comma(str3)){x=8;str3=str3.substr(0,str3.length()-2);}
            else x=0;
            pgct=hex_dec(laddr)+3;
            e=0;
            if(str3[0]=='#')
            {
                //x=0;
                str3=str3.substr(1);
                string opt;
                opt=dec_hex(hex_dec(opcode[str2])+1);

                string part2;
                try
                {
                            b=0;p=2;
                            part2 = symbtab.at(str3);
                            //cout<<"part2  "<<part2<<endl;
                            //cout<<"pcounter  "<<dec_hex(pgct)<<endl;
                            idisp=(hex_dec(part2)-pgct);
                            if(pc_is_possible(idisp))
                            {
                                if(idisp>0)
                                disp = dec_hex(hex_dec(part2)-pgct);
                                else disp=dec_hex(4096+(hex_dec(part2)-pgct));
                                disp = maketol(disp,3);
                                ans=dec_hex(x+b+p+e);
                                str=opt+ans+disp;
                            }
                            else
                            {
                                b=4;p=0;
                                idisp=(idisp+pgct)-ibase;
                                if(idisp>0)
                                disp = dec_hex(hex_dec(part2)-ibase);
                                else disp=dec_hex(4096+(hex_dec(part2)-ibase));
                                disp = maketol(disp,3);
                                ans=dec_hex(x+b+p+e);
                                str=opt+ans+disp;
                            }
                }
                catch(const out_of_range& oor)
                {
                            str3=dec_hex(dec_dec1(str3));
                            str3=maketol(str3,4);
                            str=opt+str3;
                }
            }
            else if(str3[0]=='@')
            {
                b=0;p=2;
                str3=str3.substr(1);
                string opt;
                opt=dec_hex(hex_dec(opcode[str2])+2);
                string part2;
                part2 = symbtab.at(str3);
                idisp=(hex_dec(part2)-pgct);
                if(pc_is_possible(idisp))
                {
                    b=0;p=2;
                    if(idisp>0)
                    disp = dec_hex(hex_dec(part2)-pgct);
                    else disp=dec_hex(4096+(hex_dec(part2)-pgct));
                    disp = maketol(disp,3);
                    ans=dec_hex(x+b+p+e);
                    str=opt+ans+disp;
                }
                else
                {
                            b=4;p=0;
                            idisp=(idisp+pgct)-ibase;
                            if(idisp>0)
                            disp = dec_hex(hex_dec(part2)-ibase);
                            else disp=dec_hex(4096+(hex_dec(part2)-ibase));
                            disp = maketol(disp,3);
                            ans=dec_hex(x+b+p+e);
                            str=opt+ans+disp;
                }
            }
            else
            {
                string opt;
                opt=dec_hex(hex_dec(opcode[str2])+3);

                string part2;
                try
                {
                            b=0;p=2;
                            part2 = symbtab.at(str3);
                            idisp=(hex_dec(part2)-pgct);
                            if(pc_is_possible(idisp))
                            {
                                if(idisp>0)
                                disp = dec_hex(hex_dec(part2)-pgct);
                                else disp=dec_hex(4096+(hex_dec(part2)-pgct));
                                disp = maketol(disp,3);
                                ans=dec_hex(x+b+p+e);
                                str=opt+ans+disp;
                            }
                            else
                            {
                                b=4;p=0;
                                idisp=(idisp+pgct)-ibase;
                                if(idisp>=0)
                                disp = dec_hex(hex_dec(part2)-ibase);
                                else disp=dec_hex(4096+(hex_dec(part2)-ibase));
                                disp = maketol(disp,3);
                                ans=dec_hex(x+b+p+e);
                                str=opt+ans+disp;
                            }
                }
                catch(const out_of_range& oor)
                {
                            cout<<str3<<" isn't found in symbtab"<<endl;
                }
            }
            str=maketol(str,6);
            if(counter<=27)
            {
                    adr=adr+str;
                    counter=counter+3;
            }
            else
            {
                    len=dec_hex(hex_dec(laddr)-hex_dec(start));
                    out<<"T"<<start<<len<<adr<<endl;
                    counter=3;
                    adr=str;
                    start=laddr;
                    start=maketol(start,6);
            }
        }
        else if(form=="4")
        {
            if(find_comma(str3)){x=8;str3=str3.substr(0,str3.length()-2);}
            else x=0;
            str2=str2.substr(1);
            pgct=hex_dec(laddr)+4;
            e=1;b=0;p=0;
            if(str3[0]=='#')
            {
                str3=str3.substr(1);
                string opt;
                opt=dec_hex(hex_dec(opcode[str2])+1);

                string part2;
                try
                {
                            part2 = symbtab.at(str3);
                            disp=part2;
                            disp = maketol(disp,5);
                            ans=dec_hex(x+b+p+e);
                            str=opt+ans+disp;
                            string akash=dec_hex(hex_dec(laddr)+1);
                            akash=maketol(akash,6);
                            modification[ct]="M"+akash+"05";
                            ct++;

                }
                catch(const out_of_range& oor)
                {
                            str3=dec_hex(dec_dec1(str3));
                            str3=maketol(str3,5);
                            ans=dec_hex(x+b+p+e);
                            str=opt+ans+str3;
                }
            }
            else if(str3[0]=='@')
            {
                string opt;
                opt=dec_hex(hex_dec(opcode[str2])+2);
                string part2;
                part2 = symbtab.at(str3);
                part2 = symbtab.at(str3);
                disp=part2;
                disp = maketol(disp,5);
                ans=dec_hex(x+b+p+e);
                str=opt+ans+disp;
                string akash=dec_hex(hex_dec(laddr)+1);
                akash=maketol(akash,6);
                modification[ct]="M"+akash+"05";
                ct++;

            }
            else
            {
                string opt;
                opt=dec_hex(hex_dec(opcode[str2])+3);

                string part2;
                try
                {
                            part2 = symbtab.at(str3);
                            disp=part2;
                            disp = maketol(disp,5);
                            ans=dec_hex(x+b+p+e);
                            str=opt+ans+disp;
                }
                catch(const out_of_range& oor)
                {
                            str3=dec_hex(dec_dec1(str3));
                            str3=maketol(str3,5);
                            str=opt+str3;
                }
                string akash=dec_hex(hex_dec(laddr)+1);
                akash=maketol(akash,6);
                modification[ct]="M"+akash+"05";
                ct++;
            }
            str=maketol(str,8);
            if(counter<=26)
            {
                    adr=adr+str;
                    counter=counter+4;
            }
            else
            {
                    len=dec_hex(hex_dec(laddr)-hex_dec(start));
                    out<<"T"<<start<<len<<adr<<endl;
                    counter=4;
                    adr=str;
                    start=dec_hex(hex_dec(laddr));
                    start=maketol(start,6);
            }
        }
        padr=adr;
        //out<<str<<endl;;
        str="";
    }
    while(str2!="END");
}

int main()
{
    form_mnemonic();
    form_optab();
    pass1();
    pass2();
}
