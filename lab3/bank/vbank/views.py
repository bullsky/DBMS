from django.shortcuts import render
from django.http import HttpResponseRedirect, HttpResponse
from .models import *
from django.db.models import Sum
import datetime
from django.db import connection


# Create your views here.

def index(request):
    return render(request, 'vbank/index.html')


def regist(request):
    err_msg = {}
    if request.method == "POST":
        username = request.POST.get('username')
        idnumber = request.POST.get('idnumber')
        bankcard = request.POST.get('bankcard')
        print(username)
        print(idnumber)
        print(bankcard)
        try:
            Account.objects.get(identity__id_card_num=idnumber, acc_name=username)
            err_msg['error'] = '该用户已注册！'
        except:
            try:
                BankCard.objects.get(card_num=bankcard, identity__id_card_num=idnumber)
                request.session['bankcard'] = bankcard
                request.session['username'] = username
                request.session['idnumber'] = idnumber
                return render(request, 'vbank/regist/gonext.html')
            except:
                err_msg['error'] = "身份信息或银行卡号错误!"

    return render(request, 'vbank/regist/register.html', {"error": err_msg})


def regist_next(request):
    err_msg = {}
    username = request.session.get('username')
    bankcard = request.session.get('bankcard')
    idnumber = request.session.get('idnumber')
    paycode = request.POST.get("paycode")
    phonenum = request.POST.get('phonenum')
    print(username)
    print(bankcard)
    print(paycode)
    print(phonenum)
    try:
        card = UserCard.objects.get(bank_card__card_num=bankcard, identity__id_card_num=idnumber)
        print(card.phonenum)
        index = card.phonenum.find(phonenum)
        print(index)
        if (paycode == card.paycode) & (index == 7):
            return render(request, 'vbank/regist/setting.html')
        else:
            err_msg['error'] = '付款码或手机号输入错误！'
    except:
        err_msg['error'] = '用户账户不存在！'
    context = {"error": err_msg}
    return render(request, 'vbank/regist/gonext.html', context)


def regist_set(request):
    err_msg = {}
    username = request.session.get('username')
    bankcard = request.session.get('bankcard')
    idnumber = request.session.get('idnumber')
    password = request.POST.get('password')
    repeatpasswd = request.POST.get('repeatpasswd')
    if password == repeatpasswd:
        id = Identity.objects.get(id_card_num=idnumber)
        try:
            account = Account.objects.create(identity=id, acc_name=username, passwd=password,
                                             regist_data=timezone.now(),
                                             last_login=timezone.now())
            AcountLimit.objects.create(acc_id=account)
            del request.session['username']
            del request.session['bankcard']
            del request.session['idnumber']
            return render(request, 'vbank/index.html')
        except:
            err_msg['error'] = '该用户以注册！'
    else:
        err_msg['error'] = '密码不一致！'
    context = {"error": err_msg}
    return render(request, 'vbank/regist/setting.html', context)


def login(request):
    err_msg = {}
    if request.method == "POST":
        username = request.POST.get('username')
        idnumber = request.POST.get('idnumber')
        password = request.POST.get('password')
        print("用户名：%s 密码：%s\n" % (username, password))
        try:
            account = Account.objects.get(identity__id_card_num=idnumber, acc_name=username)
            if account.status == 2:
                err_msg['error'] = '账户被锁定！请持有效证件到线下站点解锁'
            elif password == account.passwd:
                account.last_login = timezone.now()
                account.save()
                print(idnumber)
                request.session['username'] = username
                request.session['uid'] = account.acc_uid
                request.session['idnumber'] = idnumber
                request.session.set_expiry(6000)

                LoginLog.objects.create(acc_id=account, login_time=timezone.now())

                return render(request, 'vbank/home.html')
            else:
                count = account.login_fail + 1
                # if count == 3:
                #     account.status = 2

                account.login_fail = count
                account.save()
                print(count)
                LoginLog.objects.create(acc_id=account, login_time=timezone.now(), login_status=2)
                err_msg['error'] = '用户名或密码输入错误!一天之内输入错误3次账户将被锁定！'
        except:
            err_msg['error'] = '用户名或密码输入错误!一天之内输入错误3次账户将被锁定！'
    context = {"error": err_msg}
    return render(request, 'vbank/login.html', context)


def home(request):
    return render(request, 'vbank/home.html')


def balance(request):
    uid = request.session.get('uid')
    id_num = Account.objects.get(acc_uid=uid).identity.id_card_num
    balance_list = Balance.objects.filter(id_card_num=id_num)
    print(balance_list)

    context = {'balance_list': balance_list}
    return render(request, 'vbank/balance.html', context)


def transfer(request):
    err_msg = {}
    if request.method == "POST":
        bankcard = request.POST.get('bankcard')
        payaccount = request.POST.get('payaccount')
        paybankcard = request.POST.get('paybankcard')
        money = request.POST.get('money')
        request.session['bankcard'] = bankcard
        request.session['payaccount'] = payaccount
        request.session['paybankcard'] = paybankcard
        request.session['money'] = money
        print(bankcard)
        print(payaccount)
        print(paybankcard)
        print(money)
        # 检查转账账户是否存在
        # try:
        account = Account.objects.get(identity__name=payaccount).identity
        print(account)
        card = BankCard.objects.get(card_num=paybankcard)
        print(card)
        UserCard.objects.get(bank_card=card, identity=account)
        uid = request.session.get('uid')
        print(datetime.date.today().year)
        transout_sum = Flow.objects.filter(datetime__year=datetime.date.today().year,
                                           datetime__month=datetime.date.today().month,
                                           datetime__day=datetime.date.today().day,
                                           s_account=uid).annotate(sum_amount=Sum('amount'))
        transout_num = Flow.objects.filter(datetime__year=datetime.date.today().year,
                                           datetime__month=datetime.date.today().month,
                                           datetime__day=datetime.date.today().day,
                                           s_account=uid).count()
        acc_limit = AcountLimit.objects.get(acc_id=uid)
        print(transout_num)
        print(transout_sum[0].sum_amount)
        if (transout_num == 0 and int(money) < acc_limit.mtotal_amount_transout) or (
                transout_num < acc_limit.mnum_transout and (
                transout_sum[0].sum_amount + int(money)) < acc_limit.mtotal_amount_transout):
            return render(request, 'vbank/transfer/gonext.html')
        else:
            err_msg['error'] = '今日转账次数过多或超过总额限制！'
        card_list = []
        idnum = request.session.get('idnumber')
        bankcards = UserCard.objects.filter(identity__id_card_num=idnum)
        for bankcard in bankcards:
            card = bankcard.bank_card
            card_list.append(card.card_num)
        context = {'error': err_msg, "card_list": card_list}
        return render(request, 'vbank/transfer/transfer.html', context)
    else:
        card_list = []
        idnum = request.session.get('idnumber')
        print(idnum)
        bankcards = UserCard.objects.filter(identity__id_card_num=idnum)
        for bankcard in bankcards:
            card = bankcard.bank_card
            card_list.append(card.card_num)
        return render(request, 'vbank/transfer/transfer.html', {"card_list": card_list})


def transfer_next(request):
    err_msg = {}
    if request.method == "POST":
        username = request.session.get('username')
        bankcard = request.session.get('bankcard')
        payaccount = request.session.get('payaccount')
        paybankcard = request.session.get('paybankcard')
        money = request.session.get('money')
        paycode = request.POST.get('paycode')
        print(paycode)
        print(bankcard)
        print(username)
        print(payaccount)
        print(paybankcard)
        print(money)
        # try:
        if paycode == UserCard.objects.get(bank_card__card_num=bankcard).paycode:
            # 转账
            usercard = BankCard.objects.get(card_num=bankcard)
            paycard = BankCard.objects.get(card_num=paybankcard)
            print(usercard.balance)
            print(paycard.balance)
            userbalance = usercard.balance - int(money)
            if userbalance > 0:
                paybalance = paycard.balance + int(money)
                usercard.balance = userbalance
                paycard.balance = paybalance
                usercard.save()
                paycard.save()
                saccount = Account.objects.get(acc_name=username)
                daccount = Account.objects.get(acc_name=payaccount)
                Flow.objects.create(datetime=timezone.now(), s_account=saccount, s_bankcard=usercard,
                                    amount=money, d_account=daccount, d_bankcard=paycard)
                del request.session['bankcard']
                del request.session['payaccount']
                del request.session['paybankcard']
                del request.session['money']
                return render(request, 'vbank/transfer/result.html')
            else:
                del request.session['bankcard']
                del request.session['payaccount']
                del request.session['paybankcard']
                del request.session['money']
                err_msg['error'] = "余额不足！"
                return render(request, 'vbank/transfer/result.html', {'error': err_msg})
        else:
            err_msg['error'] = "付款码错误!"

        return render(request, 'vbank/transfer/gonext.html', {"error": err_msg})


def logout(request):
    try:
        uid = request.session.get('uid')
        LogoutLog.objects.create(acc_id=uid, logout_time=timezone.now())
        del request.session['uid']
        del request.session['username']
        del request.session['idnumber']
        return render(request, 'vbank/index.html')
    except:
        return render(request, '404.html')
        pass


def changepass(request):
    err_msg = {}
    if request.method == "POST":
        uid = request.session.get('uid')
        oldpasswd = request.POST.get("oldpasswd")
        account = Account.objects.get(acc_uid=uid)
        if account.passwd == oldpasswd:
            return render(request, 'vbank/changepass/setnewpass.html')
        else:
            err_msg['error'] = '密码错误！'
    context = {'error': err_msg}
    return render(request, 'vbank/changepass/changepass.html', context)


def setnewpass(request):
    err_msg = {}
    newpasswd = request.POST.get('newpasswd')
    repeatpasswd = request.POST.get('repeatpasswd')
    if newpasswd == repeatpasswd:
        uid = request.session.get('uid')
        account = Account.objects.get(acc_uid=uid)
        account.passwd = newpasswd
        account.save()
        del request.session['uid']
        del request.session['username']
        del request.session['idnumber']
        return render(request, 'vbank/index.html')
    else:
        err_msg['error'] = '密码不一致！'
    context = {'error': err_msg}
    return render(request, 'vbank/changepass/setnewpass.html', context)


class Flower:
    serial = ''
    datetime = ''
    s_account = ''
    s_bankcard = ''
    currency = ''
    amount = ''
    ftype = ''
    d_account = ''
    d_bankcard = ''

    def __init__(self, serial, datetime, s_account, s_bankcard, currency, amount, ftype, d_account, d_bankcard):
        self.serial = serial
        self.datetime = datetime
        self.s_account = s_account
        self.s_bankcard = s_bankcard
        self.currency = currency
        self.amount = amount
        self.ftype = ftype
        self.d_account = d_account
        self.d_bankcard = d_bankcard


def flow(request):
    uid = request.session.get('uid')
    out_flow_list = Flow.objects.filter(s_account__acc_uid=uid)
    in_flow_list = Flow.objects.filter(d_account__acc_uid=uid)
    flowlist = in_flow_list | out_flow_list
    flow_list = []
    for flow in flowlist:
        serial = flow.serial
        datetime = flow.datetime
        s_account = flow.s_account.acc_name
        s_bankcard = flow.s_bankcard.card_num
        currency_list = {1: "人民币", 2: '美元', 3: '欧元'}
        currency = currency_list[flow.currency]
        amount = flow.amount
        ftype_list = {1: '储蓄卡转账', 2: '电子交易', 3: '信用卡透支'}
        ftype = ftype_list[flow.type]
        d_account = flow.d_account.acc_name
        d_bankcard = flow.d_bankcard.card_num
        f = Flower(serial, datetime, s_account, s_bankcard, currency, amount, ftype, d_account, d_bankcard)
        flow_list.append(f)

    context = {'flow_list': flow_list}

    return render(request, 'vbank/flow.html', context)


class Card:
    card_num = ''
    regist_time = ''
    id_num = ''

    def __init__(self, card_num, regist_time, id_num):
        self.card_num = card_num
        self.regist_time = regist_time
        self.id_num = id_num


def unbindcard(request):
    balance_list = []
    idnum = request.session.get('idnumber')
    # cursor = connection.cursor()
    # sql = "select * from vbank_usercard where identity_id = (select id_card_num from vbank_identity where id_card_num = '" + idnum + "')"
    # cursor.execute(sql)
    # bankcards = cursor.fetchall()
    bankcards = UserCard.objects.filter(identity__id_card_num=idnum)
    print(bankcards)
    for bankcard in bankcards:
        print(bankcard)
        # card = Card(bankcard[4], bankcard[3], bankcard[6])
        card = bankcard.bank_card
        # card = bankcard
        balance_list.append(card)
    context = {"balance_list": balance_list}
    return render(request, 'vbank/unbind.html', context)


def unbind(request):
    err_msg = {}
    balance_list = []
    bankcard = request.GET.get('bankcard')
    print(bankcard)
    idnum = request.session.get('idnumber')
    bankcards = UserCard.objects.filter(identity__id_card_num=idnum)
    if bankcards.count() == 1:
        print(bankcards.count())
        err_msg['error'] = "不能解除绑定，至少绑定一张银行卡！"
    else:
        card = BankCard.objects.get(card_num=bankcard)
        UserCard.objects.get(identity__id_card_num=idnum, bank_card=card).delete()
    cursor = connection.cursor()
    sql = "select * from vbank_usercard where identity_id = (select id_card_num from vbank_identity where id_card_num = '" + idnum + "')"
    cursor.execute(sql)
    bankcards = cursor.fetchall()
    for bankcard in bankcards:
        # card = bankcard.bank_card
        card = Card(bankcard[4], bankcard[3], bankcard[6])
        balance_list.append(card)
    content = {"balance_list": balance_list, 'error': err_msg}
    return render(request, 'vbank/unbind.html', content)
