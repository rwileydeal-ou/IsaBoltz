from cmath import sqrt
import numpy as np
import matplotlib.pyplot as plt 
import matplotlib.ticker
matplotlib.rcParams['text.usetex'] = True
matplotlib.rcParams['text.latex.preamble'] = [r'\usepackage{amsmath}'] #for \text command

class Plotter:

    # convention:
    # data[0] is x-axis
    # data[1] is y-axis
    # data[2] is legend label
    # data[3] is linestyle
    def make_plot(data, title, xlabel, ylabel, filename, xMin=None, xMax=None, yMin=None, yMax=None):
        fig = plt.figure(figsize=(8,6))
        ax = fig.add_axes([0.1, 0.1, 0.6, 0.75])
        axisFontSize = 14
        ax.set_xlabel(xlabel, fontsize=axisFontSize)
        ax.set_ylabel(ylabel, fontsize=axisFontSize)
        ax.tick_params(axis="x", labelsize=axisFontSize-2)
        ax.tick_params(axis="y", labelsize=axisFontSize-2)
        ax.set_title(title)

        xaxis_Mins = []
        xaxis_Maxes = []
        yaxis_Mins = []
        yaxis_Maxes = []

        for d in data:
            if len(d) < 3:
                raise ValueError("Need at least 3 args!")
            elif len(d) == 3:
                ax.plot(d[0], d[1], label=d[2], linewidth=1.25)
            elif len(d) == 4:
                ax.plot(d[0], d[1], label=d[2], linestyle=d[3], linewidth=1.25)
            else:
                ax.plot(d[0], d[1], label=d[2], linestyle=d[3], color=d[4], linewidth=1.25)
            xaxis_Mins.append( min( d[0] ) )
            xaxis_Maxes.append( max( d[0] ) )
            yaxis_Mins.append( min( d[1] ) )
            yaxis_Maxes.append( max( d[1] ) )

        ax.legend(loc='upper left', bbox_to_anchor=(1.05, 1.), ncol=1, fancybox=True, shadow=True, fontsize='small', prop={'size':axisFontSize-3})
        ax.set_xscale('log')
        ax.set_yscale('log')
        if xMin != None and xMax != None:
            plt.xlim( xmin=xMin, xmax=xMax )
        else:
            ax.set_xlim( min( xaxis_Mins ), max( xaxis_Maxes ) )
        if yMin != None and yMax != None:
            plt.ylim( ymin=yMin, ymax=yMax )
    #    ax.set_ylim( min( yaxis_Mins ), max( yaxis_Maxes ) )
        plt.savefig(filename)

    def make_scatter_heat_plot( data, title, xlabel, ylabel, filename, drawLine=None, xMin=None, xMax=None, yMin=None, yMax=None ):
        fig = plt.figure(figsize=(8,6))
        ax = fig.add_axes([0.1, 0.1, 0.6, 0.75])
        axisFontSize = 14
        ax.set_xlabel(xlabel, fontsize=axisFontSize)
        ax.set_ylabel(ylabel, fontsize=axisFontSize)
        ax.tick_params(axis="x", labelsize=axisFontSize-2)
        ax.tick_params(axis="y", labelsize=axisFontSize-2)
        ax.set_title(title)

        xaxis_Mins = []
        xaxis_Maxes = []
        yaxis_Mins = []
        yaxis_Maxes = []

        for d in data:
            if len(d) < 3:
                raise ValueError("Need at least 3 args!")
            else:
                sc = ax.scatter(d[0], d[1], c=d[2], s=4, cmap='viridis')
            xaxis_Mins.append( min( d[0] ) )
            xaxis_Maxes.append( max( d[0] ) )
            yaxis_Mins.append( min( d[1] ) )
            yaxis_Maxes.append( max( d[1] ) )
        
        plt.colorbar(sc)

        if drawLine != None:
            ax.plot( drawLine[0], drawLine[1], linestyle='--' )
    #    ax.legend(loc='upper left', bbox_to_anchor=(1.05, 1.), ncol=1, fancybox=True, shadow=True, fontsize='small', prop={'size':axisFontSize-3})
        ax.set_xscale('log')
        ax.set_yscale('log')
        if xMin != None and xMax != None:
            plt.xlim( xmin=xMin, xmax=xMax )
        else:
            ax.set_xlim( min( xaxis_Mins ), max( xaxis_Maxes ) )
        if yMin != None and yMax != None:
            plt.ylim( ymin=yMin, ymax=yMax )
    #    ax.set_ylim( min( yaxis_Mins ), max( yaxis_Maxes ) )
        plt.savefig(filename)
