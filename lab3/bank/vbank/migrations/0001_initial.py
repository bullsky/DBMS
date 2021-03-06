# Generated by Django 2.0.5 on 2018-05-12 21:55

import datetime
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Balance',
            fields=[
                ('id', models.AutoField(primary_key=True, serialize=False)),
                ('name', models.CharField(max_length=32)),
                ('id_card_num', models.CharField(max_length=19)),
                ('card_num', models.CharField(max_length=19)),
                ('balance', models.IntegerField(default=0)),
                ('currency', models.IntegerField(choices=[(1, '人民币'), (2, '美元'), (3, '欧元')], default=1)),
            ],
            options={
                'db_table': 'view_balance',
                'managed': False,
            },
        ),
        migrations.CreateModel(
            name='Account',
            fields=[
                ('acc_uid', models.AutoField(primary_key=True, serialize=False)),
                ('acc_name', models.CharField(db_index=True, max_length=32, unique=True)),
                ('type', models.IntegerField(choices=[(1, '普通用户'), (2, 'VIP')], default=1)),
                ('passwd', models.CharField(db_index=True, max_length=128)),
                ('regist_data', models.DateTimeField()),
                ('last_login', models.DateTimeField()),
                ('login_fail', models.IntegerField(choices=[(0, '0'), (1, '1'), (2, '2'), (3, '3')], default=0)),
                ('status', models.IntegerField(choices=[(1, '正常'), (2, '锁定')], default=1)),
            ],
            options={
                'verbose_name': '用户账户',
            },
        ),
        migrations.CreateModel(
            name='AcountLimit',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('mnum_transout', models.IntegerField(default=10)),
                ('mtotal_amount_transout', models.IntegerField(default=10000000)),
                ('mnum_transin', models.IntegerField(default=10)),
                ('mtotal_amount_transin', models.IntegerField(default=10000000)),
                ('m_overdraft', models.IntegerField(default=10000)),
                ('acc_id', models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, to='vbank.Account')),
            ],
            options={
                'verbose_name': '账户限额',
            },
        ),
        migrations.CreateModel(
            name='Bank',
            fields=[
                ('bankid', models.AutoField(primary_key=True, serialize=False)),
                ('bname', models.CharField(db_index=True, max_length=32, unique=True)),
                ('headquarter_addr', models.CharField(default='中国上海浦东新区陆家嘴', max_length=32)),
                ('setup_time', models.DateField(default=datetime.datetime(2018, 5, 12, 21, 55, 30, 966670))),
                ('president', models.CharField(default='易纲', max_length=16)),
            ],
            options={
                'verbose_name': '银行',
            },
        ),
        migrations.CreateModel(
            name='BankCard',
            fields=[
                ('card_num', models.CharField(max_length=19, primary_key=True, serialize=False)),
                ('balance', models.BigIntegerField(db_index=True, default=0)),
                ('currency', models.IntegerField(choices=[(1, '人民币'), (2, '美元'), (3, '欧元')], default=1)),
                ('card_type', models.IntegerField(choices=[(1, '储蓄卡'), (2, '信用卡')], default=1)),
            ],
            options={
                'verbose_name': '银行卡',
            },
        ),
        migrations.CreateModel(
            name='BranchBank',
            fields=[
                ('branchid', models.AutoField(primary_key=True, serialize=False)),
                ('bbname', models.CharField(db_index=True, max_length=32, unique=True)),
                ('address', models.CharField(default='中国哈尔滨市香坊区珠江路104-1号银行大厦', max_length=32)),
                ('email', models.EmailField(default='2904349402@qq.com', max_length=254)),
                ('manager', models.CharField(default='刘路', max_length=16)),
                ('bankid', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='vbank.Bank')),
            ],
            options={
                'verbose_name': '支行',
            },
        ),
        migrations.CreateModel(
            name='Flow',
            fields=[
                ('serial', models.AutoField(primary_key=True, serialize=False)),
                ('datetime', models.DateTimeField()),
                ('currency', models.IntegerField(choices=[(1, '人民币'), (2, '美元'), (3, '欧元')], default=1)),
                ('amount', models.BigIntegerField(db_index=True, default=0)),
                ('type', models.IntegerField(choices=[(1, '储蓄卡转账'), (2, '电子交易'), (3, '信用卡透支')], default=1)),
                ('d_account', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='d_account', to='vbank.Account')),
                ('d_bankcard', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='d_bankcard', to='vbank.BankCard')),
                ('s_account', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='s_account', to='vbank.Account')),
                ('s_bankcard', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='s_bankcard', to='vbank.BankCard')),
            ],
            options={
                'verbose_name': '流水表',
            },
        ),
        migrations.CreateModel(
            name='Identity',
            fields=[
                ('name', models.CharField(db_index=True, max_length=32)),
                ('id_card_num', models.CharField(max_length=18, primary_key=True, serialize=False)),
                ('birthday', models.DateField(default=datetime.datetime(2018, 5, 12, 21, 55, 30, 956166))),
                ('sex', models.IntegerField(choices=[(1, '男'), (2, '女')], default=1)),
                ('age', models.CharField(default='20', max_length=3)),
                ('address', models.CharField(default='中国', max_length=32)),
            ],
            options={
                'verbose_name': '身份证',
            },
        ),
        migrations.CreateModel(
            name='LoginLog',
            fields=[
                ('logid', models.AutoField(primary_key=True, serialize=False)),
                ('login_time', models.DateTimeField()),
                ('login_ip', models.GenericIPAddressField(default='127.0.0.1')),
                ('acc_id', models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, to='vbank.Account')),
            ],
            options={
                'verbose_name': '登陆日志',
            },
        ),
        migrations.CreateModel(
            name='LogoutLog',
            fields=[
                ('logid', models.AutoField(primary_key=True, serialize=False)),
                ('logout_time', models.DateTimeField()),
                ('logout_ip', models.GenericIPAddressField(default='127.0.0.1')),
                ('acc_id', models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, to='vbank.Account')),
            ],
            options={
                'verbose_name': '登出日志',
            },
        ),
        migrations.CreateModel(
            name='UserCard',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('paycode', models.CharField(db_index=True, default='123456', max_length=6)),
                ('phonenum', models.CharField(default='17745135598', max_length=11)),
                ('regist_time', models.DateTimeField()),
                ('bank_card', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='vbank.BankCard')),
                ('bracnch_bank', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='vbank.BranchBank')),
                ('identity', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='vbank.Identity')),
            ],
            options={
                'verbose_name': '用户银行卡',
            },
        ),
        migrations.AddField(
            model_name='bankcard',
            name='identity',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='vbank.Identity'),
        ),
        migrations.AddField(
            model_name='account',
            name='identity',
            field=models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, to='vbank.Identity'),
        ),
        migrations.AlterUniqueTogether(
            name='usercard',
            unique_together={('identity', 'bank_card')},
        ),
    ]
