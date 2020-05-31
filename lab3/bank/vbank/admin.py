from django.contrib import admin

# Register your models here.
from .models import *

admin.site.register(Identity)
admin.site.register(Bank)
admin.site.register(BranchBank)
admin.site.register(BankCard)
admin.site.register(UserCard)
admin.site.register(Account)
admin.site.register(AcountLimit)
admin.site.register(Flow)
admin.site.register(LoginLog)
admin.site.register(LogoutLog)