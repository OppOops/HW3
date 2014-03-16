.class public LBenchmark;
.super Ljava/lang/Object;
.source "Benchmark.java"


# direct methods
.method public constructor <init>()V
    .registers 1

    .prologue
    .line 1
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method

.method public static main([Ljava/lang/String;)V
    .registers 9
    .parameter

    .prologue
    const/16 v7, 0x3e8

    const/4 v1, 0x0

    .line 6
    .line 7
    invoke-static {}, Ljava/lang/System;->currentTimeMillis()J

    move-result-wide v5

    move v4, v1

    move v0, v1

    .line 8
    :goto_9
    if-ge v4, v7, :cond_20

    move v3, v1

    .line 9
    :goto_c
    if-ge v3, v7, :cond_1c

    move v2, v0

    move v0, v1

    .line 10
    :goto_10
    if-ge v0, v7, :cond_17

    .line 11
    add-int/lit8 v2, v2, 0x1

    .line 10
    add-int/lit8 v0, v0, 0x1

    goto :goto_10

    .line 9
    :cond_17
    add-int/lit8 v0, v3, 0x1

    move v3, v0

    move v0, v2

    goto :goto_c

    .line 8
    :cond_1c
    add-int/lit8 v2, v4, 0x1

    move v4, v2

    goto :goto_9

    .line 15
    :cond_20
    invoke-static {}, Ljava/lang/System;->currentTimeMillis()J

    move-result-wide v1

    .line 16
    sub-long/2addr v1, v5

    long-to-int v1, v1

    .line 17
    sget-object v2, Ljava/lang/System;->out:Ljava/io/PrintStream;

    new-instance v3, Ljava/lang/StringBuilder;

    invoke-direct {v3}, Ljava/lang/StringBuilder;-><init>()V

    const-string v4, "BenchMark result: "

    invoke-virtual {v3, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3, v1}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object v1

    const-string v3, "ms. After "

    invoke-virtual {v1, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v1

    invoke-virtual {v1, v0}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;

    move-result-object v0

    const-string v1, " iterations."

    invoke-virtual {v0, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v0

    invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v0

    invoke-virtual {v2, v0}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 18
    return-void
.end method
