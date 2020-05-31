from django.db import models
from django.utils import timezone


# Create your models here.
# 用户身份证信息
class Identity(models.Model):
    name = models.CharField(max_length=32, db_index=True)
    id_card_num = models.CharField(max_length=18, primary_key=True)
    birthday = models.DateField(default=timezone.now())
    sex_list = ((1, "男"), (2, "女"))
    sex = models.IntegerField(choices=sex_list, default=1)
    age = models.CharField(max_length=3, default='20')
    address = models.CharField(max_length=32, default='中国')

    class Meta:
        verbose_name = '身份证'

    def __unicode__(self):
        return self.id_card_num


# 银行
class Bank(models.Model):
    bankid = models.AutoField(primary_key=True)
    bname = models.CharField(max_length=32, unique=True, db_index=True)
    headquarter_addr = models.CharField(max_length=32, default='中国上海浦东新区陆家嘴')
    setup_time = models.DateField(default=timezone.now())
    president = models.CharField(max_length=16, default='易纲')

    class Meta:
        verbose_name = '银行'

    def __unicode__(self):
        return self.bname


# 支行
class BranchBank(models.Model):
    bankid = models.ForeignKey(Bank, on_delete=models.CASCADE)
    branchid = models.AutoField(primary_key=True)
    bbname = models.CharField(max_length=32, unique=True, db_index=True)
    address = models.CharField(max_length=32, default='中国哈尔滨市香坊区珠江路104-1号银行大厦')
    email = models.EmailField(default='2904349402@qq.com')
    manager = models.CharField(max_length=16, default='刘路')

    class Meta:
        verbose_name = '支行'

    def __unicode__(self):
        return self.branchid


# 银行卡
class BankCard(models.Model):
    identity = models.ForeignKey(Identity, on_delete=models.CASCADE)
    card_num = models.CharField(max_length=19, primary_key=True)
    balance = models.BigIntegerField(default=0, db_index=True)
    currency_list = ((1, "人民币"), (2, "美元"), (3, "欧元"))
    currency = models.IntegerField(choices=currency_list, default=1)
    card_type_list = ((1, "储蓄卡"), (2, "信用卡"))
    card_type = models.IntegerField(choices=card_type_list, default=1)

    class Meta:
        verbose_name = '银行卡'

    def __unicode__(self):
        return self.card_num


# 用户银行卡
class UserCard(models.Model):
    identity = models.ForeignKey(Identity, on_delete=models.CASCADE)
    bank_card = models.ForeignKey(BankCard, on_delete=models.CASCADE)
    bracnch_bank = models.ForeignKey(BranchBank, on_delete=models.CASCADE)
    paycode = models.CharField(max_length=6, db_index=True, default='123456')
    phonenum = models.CharField(max_length=11, default='17745135598')
    regist_time = models.DateTimeField()

    class Meta:
        verbose_name = '用户银行卡'
        unique_together = ('identity', 'bank_card')

    def __unicode__(self):
        return self.id


# 用户账户
class Account(models.Model):
    acc_uid = models.AutoField(primary_key=True)
    identity = models.OneToOneField(Identity, on_delete=models.CASCADE)
    acc_name = models.CharField(max_length=32, db_index=True, unique=True)
    type_list = ((1, "普通用户"), (2, "VIP"))
    type = models.IntegerField(choices=type_list, default=1)
    passwd = models.CharField(max_length=128, db_index=True)
    regist_data = models.DateTimeField()
    last_login = models.DateTimeField()
    count = ((0, "0"), (1, "1"), (2, "2"), (3, "3"))
    login_fail = models.IntegerField(choices=count, default=0)
    status_list = ((1, "正常"), (2, "锁定"))
    status = models.IntegerField(choices=status_list, default=1)

    class Meta:
        verbose_name = '用户账户'

    def __unicode__(self):
        return self.acc_uid


# 账户限制
class AcountLimit(models.Model):
    acc_id = models.OneToOneField(Account, on_delete=models.CASCADE)
    # 一天最大取款次数
    mnum_transout = models.IntegerField(default=10)
    # 一天最大取款总额
    mtotal_amount_transout = models.IntegerField(default=10000000)
    # 一天最大存款次数
    mnum_transin = models.IntegerField(default=10)
    # 一天最大存款数额
    mtotal_amount_transin = models.IntegerField(default=10000000)
    # 信用卡额度
    m_overdraft = models.IntegerField(default=10000)

    class Meta:
        verbose_name = "账户限额"


# 流水表
class Flow(models.Model):
    serial = models.AutoField(primary_key=True)
    datetime = models.DateTimeField()
    # s_account = models.CharField(max_length=16)
    # s_bankcard = models.CharField(max_length=19)
    s_account = models.ForeignKey(Account, related_name='s_account', on_delete=models.CASCADE)
    s_bankcard = models.ForeignKey(BankCard, related_name='s_bankcard', on_delete=models.CASCADE)
    currency_list = ((1, "人民币"), (2, "美元"), (3, "欧元"))
    currency = models.IntegerField(choices=currency_list, default=1)
    amount = models.BigIntegerField(default=0, db_index=True)
    # balance = models.BigIntegerField(default=0, db_index=True)
    type_list = ((1, "储蓄卡转账"), (2, "电子交易"), (3, "信用卡透支"))
    type = models.IntegerField(choices=type_list, default=1)
    # target_web = models.URLField(null=True)
    # d_account = models.CharField(max_length=16)
    # d_bankcard = models.CharField(max_length=19)
    d_account = models.ForeignKey(Account, related_name='d_account', on_delete=models.CASCADE)
    d_bankcard = models.ForeignKey(BankCard, related_name='d_bankcard', on_delete=models.CASCADE)

    class Meta:
        verbose_name = '流水表'

    def __unicode__(self):
        return self.serial


# 登陆日志

class LoginLog(models.Model):
    logid = models.AutoField(primary_key=True)
    acc_id = models.ForeignKey(Account, on_delete=models.CASCADE)
    login_time = models.DateTimeField()
    status_list = ((1, "登陆成功"), (2, "登陆失败"))
    login_status = models.IntegerField(choices=status_list, default=1)
    login_ip = models.GenericIPAddressField(default='127.0.0.1')

    class Meta:
        verbose_name = '登陆日志'
        unique_together = ('acc_id', 'login_time')

    def __unicode__(self):
        return self.acc_id


# 登出日志

class LogoutLog(models.Model):
    logid = models.AutoField(primary_key=True)
    acc_id = models.ForeignKey(Account, on_delete=models.CASCADE)
    logout_time = models.DateTimeField()
    logout_ip = models.GenericIPAddressField(default='127.0.0.1')

    class Meta:
        verbose_name = '登出日志'
        unique_together = ('acc_id', 'logout_time')

    def __unicode__(self):
        return self.acc_id


# 视图

class Balance(models.Model):
    id = models.AutoField(primary_key=True)
    name = models.CharField(max_length=32)
    id_card_num = models.CharField(max_length=19)
    card_num = models.CharField(max_length=19)
    balance = models.IntegerField(default=0)
    currency_list = ((1, "人民币"), (2, "美元"), (3, "欧元"))
    currency = models.IntegerField(choices=currency_list, default=1)

    class Meta:
        managed = False
        db_table = 'view_balance'


# class ViewBankcard(models.Model):
#     id = models.AutoField(primary_key=True)
#     card_num = models.CharField(max_length=19)
#     regist_time = models.DateTimeField()
#     bbname = models.CharField(max_length=32)
#     card_type = models.IntegerField()
#
#     class Meta:
#         managed = False
#         db_table = 'view_bankcard'
