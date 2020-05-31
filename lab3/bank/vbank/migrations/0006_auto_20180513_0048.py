# Generated by Django 2.0.5 on 2018-05-13 00:48

import datetime
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('vbank', '0005_auto_20180513_0044'),
    ]

    operations = [
        migrations.AlterField(
            model_name='bank',
            name='setup_time',
            field=models.DateField(default=datetime.datetime(2018, 5, 13, 0, 48, 19, 843913)),
        ),
        migrations.AlterField(
            model_name='flow',
            name='d_account',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='d_account', to='vbank.Account'),
        ),
        migrations.AlterField(
            model_name='flow',
            name='d_bankcard',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='d_bankcard', to='vbank.BankCard'),
        ),
        migrations.AlterField(
            model_name='flow',
            name='s_account',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='s_account', to='vbank.Account'),
        ),
        migrations.AlterField(
            model_name='flow',
            name='s_bankcard',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='s_bankcard', to='vbank.BankCard'),
        ),
        migrations.AlterField(
            model_name='identity',
            name='birthday',
            field=models.DateField(default=datetime.datetime(2018, 5, 13, 0, 48, 19, 833886)),
        ),
    ]
