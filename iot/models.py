from django.db import models

class Information(models.Model):
    air_temp = models.CharField(max_length=200)
    air_hum = models.CharField(max_length=200)
    air_pres = models.CharField(max_length=200)
    light = models.CharField(max_length=200)
    get_date = models.DateTimeField('date published')
